#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

int MPI_File_write_at_all(MPI_File mpi_fh, MPI_Offset offset, void *buf,
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
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_WRITEALL;

    ret_val =
        PMPI_File_write_at_all(mpi_fh, offset, buf, count, datatype,
                status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_write_at_all_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Status *status, MPI_Fint *ierr ) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_at_all(c_fh, *offset, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}
