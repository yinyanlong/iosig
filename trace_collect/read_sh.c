/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 * Modified on: 09/14/2011 by Yanlong Yin
 *
 */
#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

int MPI_File_read_shared(MPI_File mpi_fh, void *buf, int count,
			 MPI_Datatype datatype, MPI_Status * status)
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
    iorec->operation = MPI_READSH;
/* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    static char myname[] = "MPI_FILE_READ_SHARED";
    int datatype_size, incr;
    ADIO_Offset off, shared_fp;
    ADIO_File fh;


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
    MPIO_CHECK_COUNT_SIZE(fh, count, datatype_size, myname, error_code);
    /* --END ERROR HANDLING-- */

    if (count * datatype_size == 0) {
#ifdef HAVE_STATUS_SET_BYTES
	MPIR_Status_set_bytes(status, datatype, 0);
#endif
	error_code = MPI_SUCCESS;
	goto fn_exit;
    }


    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_INTEGRAL_ETYPE(fh, count, datatype_size, myname,
			      error_code);
    MPIO_CHECK_READABLE(fh, myname, error_code);
    MPIO_CHECK_FS_SUPPORTS_SHARED(fh, myname, error_code);
    /* --END ERROR HANDLING-- */

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;

    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);
    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
	error_code = MPIO_Err_return_file(fh, error_code);
	goto fn_exit;
    }
    /* --END ERROR HANDLING-- */

    /* contiguous or strided? */
    if (buftype_is_contig && filetype_is_contig) {
	/* convert count and shared_fp to bytes */
	bufsize = datatype_size * count;
	off = fh->disp + fh->etype_size * shared_fp;

	/* if atomic mode requested, lock (exclusive) the region, because there                                                                                                                                            
	   could be a concurrent noncontiguous request. On NFS, locking                                                                                                                                                    
	   is done in the ADIO_ReadContig. */

	if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
	    ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);

	ADIO_ReadContig(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
			off, status, &error_code);

	if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
	    ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);
    } else {
	ADIO_ReadStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
			 shared_fp, status, &error_code);
	/* For strided and atomic mode, locking is done in ADIO_ReadStrided */
    }

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

void mpi_file_read_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count,
			   MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;
    
    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);
    
    ret_val - MPI_File_read_shared(c_fh, buf, *count, c_datatype, status);
    
    *ierr = (MPI_Fint)ret_val;
}
