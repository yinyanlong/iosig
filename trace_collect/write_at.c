/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 * Modified on 09/28/2011 by Yanlong Yin.
 *
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

int MPI_File_write_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
		      int count, MPI_Datatype datatype,
		      MPI_Status * status)
{
    int ret_val;
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    iorec->file_pos = offset;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_WRITEAT;

    ret_val =
	PMPI_File_write_at(mpi_fh, offset, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

