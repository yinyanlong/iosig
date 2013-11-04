#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

int MPI_File_iread_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
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
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IREADAT;

    ret_val =
        PMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iread_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf, int *count, MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr){
    int ret_val;
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iread_at(c_fh, *offset, buf, *count, c_datatype, &c_request);
    *ierr = (MPI_Fint)ret_val;
    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
}
