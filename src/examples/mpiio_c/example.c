#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main (int argc, char *args[]) {

    int ret;
    int i;
    int myrank;
    int req_size = 256;
    int range = 32*1024;
    MPI_File datafile;
    MPI_Status status;
    char * buf;

    MPI_Init( &argc, &args );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

    printf("%s.\n", args[1]);
    ret = MPI_File_open(MPI_COMM_WORLD, args[1], MPI_MODE_CREATE | MPI_MODE_RDWR, MPI_INFO_NULL, &datafile );
    if (ret != MPI_SUCCESS) {
        printf("Failed to open data file. \n");
        MPI_Finalize();
        return -1;
    }
    /*
    printf("------------1------------\n");
    MPI_File_close(&datafile);
    MPI_Finalize();
    return 0;
    */

    if(myrank==0) {

        MPI_Barrier(MPI_COMM_WORLD);

        buf = malloc(req_size);
        if (!buf) {
            perror("Failed to malloc. \n");
            MPI_File_close(&datafile);
            MPI_Finalize();
            return;
        }

        int size_remain = range;
        MPI_File_seek(datafile, range*myrank, MPI_SEEK_SET);

        while (size_remain > req_size) {
            MPI_File_write(datafile, buf, req_size, MPI_CHAR, &status);
            size_remain -= req_size;
        }
        free(buf);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    else {
        MPI_Barrier(MPI_COMM_WORLD);

        buf = malloc(req_size);
        if (!buf) {
            perror("Failed to malloc. \n");
            MPI_File_close(&datafile);
            MPI_Finalize();
            return;
        }

        int size_remain = range;
        MPI_File_seek(datafile, range*myrank, MPI_SEEK_SET);
        
        while (size_remain > req_size) {
            MPI_File_write(datafile, buf, req_size, MPI_CHAR, &status);
            size_remain -= req_size;
        }
        free(buf);

        MPI_Barrier(MPI_COMM_WORLD);
    }
    MPI_File_close(&datafile);
    MPI_Finalize();
}

