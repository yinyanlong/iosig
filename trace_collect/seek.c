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
 *              09/28/2011 by Yanlong Yin
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"


int MPI_File_seek(MPI_File mpi_fh, MPI_Offset offset, int whence)
{
    int ret_val;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    iorec->data_size = 0;
    iorec->op_time = start;
    iorec->operation = MPI_SEEK;

    ret_val = PMPI_File_seek(mpi_fh, offset, whence);

    //iorec->file_pos = mpi_fh->fp_ind;
    iorec->file_pos = offset;

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

