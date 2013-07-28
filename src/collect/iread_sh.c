/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 * Modified on 09/14/2011 by Yanlong Yin.
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

#ifdef HAVE_MPI_GREQUEST
#include "mpiu_greq.h"

int MPI_File_iread_shared(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Request * request)
{
    /* add by huaiming */
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IREADSH;
    /* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    ADIO_File fh;
    static char myname[] = "MPI_FILE_IREAD_SHARED";
    int datatype_size, incr;
    MPI_Status status;
    ADIO_Offset off, shared_fp;
    MPI_Offset nbytes = 0;

    MPIU_THREAD_CS_ENTER(ALLFUNC,);
    MPIR_Nest_incr();

    fh = MPIO_File_resolve(mpi_fh);

    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_FILE_HANDLE(fh, myname, error_code);
    MPIO_CHECK_COUNT(fh, count, myname, error_code);
    MPIO_CHECK_DATATYPE(fh, datatype, myname, error_code);
    /* --END ERROR HANDLING-- */

    MPI_Type_size(datatype, &datatype_size);

    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_INTEGRAL_ETYPE(fh, count, datatype_size, myname,
            error_code);
    MPIO_CHECK_FS_SUPPORTS_SHARED(fh, myname, error_code);
    MPIO_CHECK_COUNT_SIZE(fh, count, datatype_size, myname, error_code);
    /* --END ERROR HANDLING-- */

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;
    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
        /* note: ADIO_Get_shared_fp should have set up error code already? */
        MPIO_Err_return_file(fh, error_code);
    }
    /* --END ERROR HANDLING-- */

    if (buftype_is_contig && filetype_is_contig) {
        /* convert count and shared_fp to bytes */
        bufsize = datatype_size * count;
        off = fh->disp + fh->etype_size * shared_fp;
        if (!(fh->atomicity)) {
            ADIO_IreadContig(fh, buf, count, datatype,
                    ADIO_EXPLICIT_OFFSET, off, request,
                    &error_code);
        } else {
            /* to maintain strict atomicity semantics with other concurrent                                                                                                                                                
               operations, lock (exclusive) and call blocking routine */

            if (fh->file_system != ADIO_NFS) {
                ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);
            }

            ADIO_ReadContig(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                    off, &status, &error_code);

            if (fh->file_system != ADIO_NFS) {
                ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);
            }
            if (error_code == MPI_SUCCESS) {
                nbytes = count * datatype_size;
            }
            MPIO_Completed_request_create(&fh, nbytes, &error_code,
                    request);
        }
    } else {
        ADIO_IreadStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                shared_fp, request, &error_code);
    }

    /* record the end time */
    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);
    return error_code;
}

void mpi_file_iread_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iread_shared(c_fh, buf, *count, c_datatype, &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

#endif

