#include "mpi.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

struct timeval bigbang;

int MPI_Init(int *argc, char ***argv)
{
    int ret_val;
    if (!bigbang.tv_sec) {
        gettimeofday(&bigbang, NULL);
    }

    ret_val = PMPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &thisrank);
    my_rank = thisrank;
    init_log(my_rank);

    //PushIO_RTB_init(thisrank);
    iorec = malloc(sizeof(iosig_mpiio_trace_record));
    return ret_val;
}


void mpi_init_ (int *ierr)
{
    int ret_val, i;
    int argc;
    char ** argv;

    argv=malloc(32*sizeof(char*));
    memset(argv, 0, 32*sizeof(char*));

    //get command line arguments from main()
    getProcCmdLine2(&argc, argv);

    /*
    printf("======\n");
    printf("# or arg: %d\n", argc);
    i=0;
    for(i=0; i<argc; i++) {
        printf("argv[%d]=\"%s\"    %u    %u\n", i, argv[i], &argv[i], argv[i]);
    }
    printf("argv[%d]=\"%s\"    %u    %u\n", i, argv[i], &argv[i], argv[i]);
    printf("======\n");
    */

    ret_val = MPI_Init(&argc, &argv);
    
    //for (i=0; i<argc; i++)
    //   free(argv[i]); 
    //free(argv);

    free(argv[0]);
    free(argv);
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
    return;
}

