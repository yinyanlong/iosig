/* This file is part of the Server-Push File Access Server (FAS) environment
 *
 *            <<<<  Add more info >>>
 ****************************************************************************
 *
 * Author:      Suren Byna (sbyna@iit.edu)
 *              Illinois Institute of Technology &
 *              Argonne National Laboratory
 * Created on:  03/09/2007
 * Modified on: 03/16/2007 by Suren Byna
 *
 * Funded by:   NSF, Award # CCF-0621435
 *
 * File name: pushio_init.c
 * Purpose  : Wrapper function for MPI_Init ()
 *            Initiates tracing.
 *
 * Modified on: 12/16/2009 by Huaiming Song
 *              09/14/2011 by Yanlong Yin
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

#include <string.h>

int MPI_File_open(MPI_Comm comm, char *filename, int amode,
		  MPI_Info info, MPI_File * fh)
{
    int ret_val;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = 0;		/*  THIS HAS TO BE FIXED   */
    iorec->data_size = 0;
    iorec->op_time = start;
    iorec->operation = MPI_OPEN;

    ret_val = PMPI_File_open(comm, filename, amode, info, fh);

    iorec->file_pos = 0;
    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_openl_(MPI_Fint *comm, char *filename, MPI_Fint *amode,
            MPI_Fint *info, MPI_Fint *fh, MPI_Fint *ierr,
            MPI_Fint *length){
    MPI_Comm c_comm;
    MPI_Info c_info;
    MPI_File c_fh;
    int ret_val, str_len, i;
    char *c_filename;
    
    c_comm = MPI_Comm_f2c(*comm);
    c_info = MPI_Info_f2c(*info);
    
    for (i=*length-1; i>=0; i--)
        if (filename[i] != ' ')
            break;
    str_len = i+1;
    
    c_filename = (char*)malloc(str_len+1);
    strncpy(c_filename, filename, str_len);
    c_filename[str_len] = '\0';
    
    ret_val = MPI_File_open(c_comm, c_filename, *amode, c_info, &c_fh);
    
    if(ret_val == MPI_SUCCESS)
        *fh = MPI_File_c2f(c_fh);
    *ierr = (MPI_Fint)ret_val;
    
    free(c_filename);
}
    
/*
void mpi_file_open_(MPI_Fint *comm, char *filename, MPI_Fint *len,
            MPI_Fint *amode, MPI_Fint *info, MPI_Fint *fh,
            MPI_Fint *ierr){
    MPI_Comm c_comm;
    MPI_Info c_info;
    MPI_File c_fh;
    int ret_val; //, str_len, i;
    char *c_name;
    
    c_comm = MPI_Comm_f2c(*comm);
    c_info = MPI_Info_f2c(*info);
    
    filename[*len] = '\0';
    
    ret_val = MPI_File_Open(c_comm, filename, *amode, c_info, &c_fh);
    if(ret_val == MPI_SUCCESS)
        *fh = MPI_File_c2f(c_fh);
    *ierr = (MPI_Fint)ret_val;
}
*/
