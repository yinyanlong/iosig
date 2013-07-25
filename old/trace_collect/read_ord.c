/*
 *	Copyright(c) by Huaiming Song 
 *	Email: huaiming.song@iit.edu
 *	12/16/2009
 *	Illinois Institute of Technology
 *	Scalable Computing Software Laboratory
 *
 * Modified on: 09/14/2011 by Yanlong Yin
 */

#include "mpioimpl.h"
#include "mpiimpl.h"
#include "pushio_trace.h"

int MPI_File_read_ordered(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Status * status)
{
    /* add by huaiming */
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
    iorec->operation = MPI_READORD;

    /* end of add. by huaiming */

    int error_code, datatype_size, nprocs, myrank, incr;
    int source, dest;
    static char myname[] = "MPI_FILE_READ_ORDERED";
    ADIO_Offset shared_fp = 0;
    ADIO_File fh;

    MPIU_THREAD_CS_ENTER(ALLFUNC,);
    MPIR_Nest_incr();

    fh = MPIO_File_resolve(mpi_fh);

    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_FILE_HANDLE(fh, myname, error_code);
    MPIO_CHECK_COUNT(fh, count, myname, error_code);
    MPIO_CHECK_DATATYPE(fh, datatype, myname, error_code);
    /* --END ERROR HANDLING-- */

    MPI_Type_size(datatype, &datatype_size);

    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_INTEGRAL_ETYPE(fh, count, datatype_size, myname,
            error_code);
    MPIO_CHECK_FS_SUPPORTS_SHARED(fh, myname, error_code);
    MPIO_CHECK_COUNT_SIZE(fh, count, datatype_size, myname, error_code);
    /* --END ERROR HANDLING-- */

    ADIOI_TEST_DEFERRED(fh, "MPI_File_read_ordered", &error_code);

    MPI_Comm_size(fh->comm, &nprocs);
    MPI_Comm_rank(fh->comm, &myrank);

    incr = (count * datatype_size) / fh->etype_size;

    /* Use a message as a 'token' to order the operations */
    source = myrank - 1;
    dest = myrank + 1;
    if (source < 0)
        source = MPI_PROC_NULL;
    if (dest >= nprocs)
        dest = MPI_PROC_NULL;
    MPI_Recv(NULL, 0, MPI_BYTE, source, 0, fh->comm, MPI_STATUS_IGNORE);

    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);
    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
        error_code = MPIO_Err_return_file(fh, error_code);
        goto fn_exit;
    }
    /* --END ERROR HANDLING-- */

    MPI_Send(NULL, 0, MPI_BYTE, dest, 0, fh->comm);

    ADIO_ReadStridedColl(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
            shared_fp, status, &error_code);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    log_read_trace(iorec);
    PushIO_RTB_log(thisrank, iorec);

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    /* FIXME: Check for error code from ReadStridedColl? */
    return error_code;
}

void mpi_file_read_ordered_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read_ordered(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}
