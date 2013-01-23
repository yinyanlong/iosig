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
    //gettimeofday(&init_tv, NULL);
    init_tv.tv_sec = bigbang.tv_sec;
    init_tv.tv_usec = bigbang.tv_usec;
    printf("iosig Init 10\n");

    printf("======\n");
    printf("# of arg: %d\n", *argc);
    int i=0;
    for(i=0; i<*argc; i++) {
          //printf(" arg %d : %s \n", *i, (char*) ( ( (char *[])(*argv))[i])        );
        char * c; // = NULL;
        //c = *((char**)   ( (*argv)+i*sizeof(char*)) );
        //printf(" argv[ %d ]: %s \n", i, c        );
        printf(" argv[ %d ]: %s  %u   %u\n", i, (*argv)[i], &(*argv)[i],   (*argv)[i]        );
    }
    printf(" argv[ %d ]: %s  %u   %u\n", i, (*argv)[i], &(*argv)[i],   (*argv)[i]        );
    
    printf("======\n");

    printf("iosig Init 10\n");
    init_log();
    printf("iosig Init 20\n");
    ret_val = PMPI_Init(argc, argv); // <--- error
    printf("iosig Init 30\n");
    MPI_Comm_rank(MPI_COMM_WORLD, &thisrank);
    printf("iosig Init 40\n");

    PushIO_RTB_init(thisrank);
    iorec = malloc(sizeof(PushIO_Trace_record));
    return ret_val;
}


int argc;
char ** argv;
void mpi_init_ (int *ierr)
{
    int ret_val, i;

    argv=malloc(32*sizeof(char*));
    memset(argv, 0, 32*sizeof(char*));

    printf("iosig init_ 10\n");
    //get command line arguments from main()
    getProcCmdLine(&argc, argv);

    printf("======\n");
    printf("# or arg: %d\n", argc);
    i=0;
    for(i=0; i<argc; i++) {
        //printf(" arg %d : %s \n", i,  argv[i]       );
        printf("argv[%d]=\"%s\"    %u    %u\n", i, argv[i], &argv[i], argv[i]);
    }
    printf("argv[%d]=NULL    %u    %u\n", i, &argv[i], argv[i]);
    printf("======\n");

    printf("iosig init_ 20\n");
    //ret_val = MPI_Init(&argc, (char ***)&argv);
    ret_val = MPI_Init(&argc, &argv);
    printf("iosig init_ 30\n");

    
    for (i=0; i<argc; i++)
       free(argv[i]); 
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
    //free(argv[0]);
    //free(argv);
    return;
}

