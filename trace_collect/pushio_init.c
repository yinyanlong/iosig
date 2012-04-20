/*
 *      Copyright(c) by Huaiming Song 
 *      Email: huaiming.song@iit.edu
 *      12/16/2009
 *      Illinois Institute of Technology
 *      Scalable Computing Software Laboratory
 *
 * Modified on: 09/20/2011 by Yanlong Yin
 */

#include "mpi.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

int MPI_Init(int *argc, char ***argv)
{
    int ret_val;
    gettimeofday(&init_tv, NULL);
    init_log();
    ret_val = PMPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &thisrank);

    PushIO_RTB_init(thisrank);
    iorec = malloc(sizeof(PushIO_Trace_record));
    return ret_val;
}

void mpi_init_ (int *ierr)
{
    int ret_val;
    int argc;
    char *argv[32];

    //get command line arguments from main()
    getProcCmdLine(&argc, argv);
    ret_val = MPI_Init(&argc, (char ***)&argv);

    *ierr = ret_val;
}


int MPI_Finalize(void)
{
    int ret_val;
    PushIO_RTB_finalize(thisrank);
    end_log();
    free(iorec);

    ret_val = PMPI_Finalize();
    return ret_val;
}

void mpi_finalize_ (int *ierr)
{

    int ret_val;
    ret_val = MPI_Finalize();
    *ierr = ret_val;
}

