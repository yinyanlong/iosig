/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 * Modified on: 09/14/2011 by Yanlong Yin
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"


int MPI_File_iwrite_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
        int count, MPI_Datatype datatype,
        MPIO_Request * request)
{
    int ret_val;
    struct timeval start, end;
    int dtsize;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    iorec->file_pos = offset;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IWRITEAT;

    ret_val =
        PMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iwrite_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *request, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iwrite_at(c_fh, *offset, buf, *count, c_datatype, 
            &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}
