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

int MPI_File_iread_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
		      int count, MPI_Datatype datatype,
		      MPIO_Request * request)
{
    int ret_val;
    struct timeval start, end;
    int dtsize;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    iorec->file_pos = offset;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IREADAT;

    ret_val =
	PMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iread_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf, int *count, MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr){
    int ret_val;
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    
    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);
    
    ret_val = MPI_File_iread_at(c_fh, *offset, buf, *count, c_datatype, &c_request);
    *ierr = (MPI_Fint)ret_val;
    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
}
