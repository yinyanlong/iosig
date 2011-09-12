/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"


int MPI_File_iwrite_shared(MPI_File mpi_fh, void *buf, int count,
			   MPI_Datatype datatype, MPIO_Request * request)
{
/* add by huaiming */
    int dtsize;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = tv;
    iorec->operation = MPI_IWRITESH;
    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);
/* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    ADIO_File fh;
    int datatype_size, incr;
    ADIO_Status status;
    ADIO_Offset off, shared_fp;
    static char myname[] = "MPI_FILE_IWRITE_SHARED";

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
    if (error_code != MPI_SUCCESS) {
	/* note: ADIO_Get_shared_fp should have set up error code already? */
	MPIO_Err_return_file(fh, error_code);
    }

    /* contiguous or strided? */
    if (buftype_is_contig && filetype_is_contig) {
	/* convert sizes to bytes */
	bufsize = datatype_size * count;
	off = fh->disp + fh->etype_size * shared_fp;
	if (!(fh->atomicity))
	    ADIO_IwriteContig(fh, buf, count, datatype,
			      ADIO_EXPLICIT_OFFSET, off, request,
			      &error_code);
	else {
	    /* to maintain strict atomicity semantics with other concurrent                                                                                                                                                
	       operations, lock (exclusive) and call blocking routine */


	    if (fh->file_system != ADIO_NFS)
		ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);

	    ADIO_WriteContig(fh, buf, count, datatype,
			     ADIO_EXPLICIT_OFFSET, off, &status,
			     &error_code);

	    if (fh->file_system != ADIO_NFS)
		ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);

	    MPIO_Completed_request_create(&fh, bufsize, &error_code,
					  request);
	}
    } else
	ADIO_IwriteStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
			   shared_fp, request, &error_code);

  fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    return error_code;
}
