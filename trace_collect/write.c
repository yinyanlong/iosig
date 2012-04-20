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

int MPI_File_write(MPI_File mpi_fh, void *buf, int count,
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
    iorec->operation = MPI_WRITE;

    ret_val = PMPI_File_write(mpi_fh, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_write_(MPI_Fint *fh, void *buf, MPI_Fint *count,
		     MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;
    
    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);
    
    ret_val = MPI_File_write(c_fh, buf, *count, c_datatype, status);
    
    *ierr = (MPI_Fint)ret_val;
}
