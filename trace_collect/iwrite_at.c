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


int MPI_File_iwrite_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
		       int count, MPI_Datatype datatype,
		       MPIO_Request * request)
{
    int ret_val;
    struct timeval tv;
    int dtsize;
    gettimeofday(&tv, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = tv;
    iorec->operation = MPI_IWRITEAT;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    ret_val =
	PMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, request);
    return ret_val;
}
