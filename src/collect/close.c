#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

int MPI_File_close(MPI_File * fh)
{
    int ret_val;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = 0;		/*  THIS HAS TO BE FIXED   */
    iorec->data_size = 0;
    iorec->op_time = start;
    iorec->operation = MPI_CLOSE;

    ret_val = PMPI_File_close(fh);
    gettimeofday(&end, NULL);
    iorec->file_pos = 0;
    iorec->op_end_time = end;
    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_close_(MPI_Fint  *fh , MPI_Fint *ierr){
    MPI_File c_fh;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    ret_val = MPI_File_close(&c_fh);

    if(ret_val == MPI_SUCCESS)
        *fh = MPI_File_c2f(c_fh);
    *ierr = (MPI_Fint)ret_val;
}

