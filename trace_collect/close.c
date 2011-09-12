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
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

int MPI_File_close(MPI_File * fh)
{
    int ret_val;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = 0;		/*  THIS HAS TO BE FIXED   */
    iorec->data_size = 0;
    iorec->op_time = tv;
    iorec->operation = MPI_CLOSE;

    ret_val = PMPI_File_close(fh);
    iorec->file_pos = 0;
    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}
