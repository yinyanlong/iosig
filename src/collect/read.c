#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

int MPI_File_read(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Status * status)
{
    int ret_val;
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
    iorec->operation = MPI_READ;

    ret_val = PMPI_File_read(mpi_fh, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_read_(MPI_Fint *fh, void *buf, int *count,  MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    int ret_val;
    MPI_File c_fh;
    MPI_Datatype c_datatype;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

