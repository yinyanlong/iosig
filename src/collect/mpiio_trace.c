#include <string.h>
#include "mpioimpl.h"
#include "mpiimpl.h"
#include "iosig_trace.h"

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

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

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

int MPI_File_close(MPI_File * fh)
{
    int ret_val;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = 0;		/*  THIS HAS TO BE FIXED   */
    iorec->data_size = 0;
    iorec->op_time = start;
    iorec->operation = MPI_CLOSE;

    ret_val = PMPI_File_close(fh);
    gettimeofday(&end, NULL);
    iorec->file_pos = 0;
    iorec->op_end_time = end;
    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_close_(MPI_Fint  *fh , MPI_Fint *ierr){
    MPI_File c_fh;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    ret_val = MPI_File_close(&c_fh);

    if(ret_val == MPI_SUCCESS)
        *fh = MPI_File_c2f(c_fh);
    *ierr = (MPI_Fint)ret_val;
}

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
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IREADAT;

    ret_val =
        PMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

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

int MPI_File_iread(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Request * request)
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
    iorec->operation = MPI_IREAD;

    ret_val = PMPI_File_iread(mpi_fh, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iread_(MPI_Fint *fh, void *buf, int *count, MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr){
    MPI_File c_fh;
    int ret_val;
    MPI_Datatype c_datatype;
    MPI_Request c_request;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iread(c_fh, buf, *count, c_datatype, &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

#ifdef HAVE_MPI_GREQUEST
#include "mpiu_greq.h"

int MPI_File_iread_shared(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Request * request)
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
    iorec->operation = MPI_IREADSH;
    /* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    ADIO_File fh;
    static char myname[] = "MPI_FILE_IREAD_SHARED";
    int datatype_size, incr;
    MPI_Status status;
    ADIO_Offset off, shared_fp;
    MPI_Offset nbytes = 0;

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

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;
    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
        /* note: ADIO_Get_shared_fp should have set up error code already? */
        MPIO_Err_return_file(fh, error_code);
    }
    /* --END ERROR HANDLING-- */

    if (buftype_is_contig && filetype_is_contig) {
        /* convert count and shared_fp to bytes */
        bufsize = datatype_size * count;
        off = fh->disp + fh->etype_size * shared_fp;
        if (!(fh->atomicity)) {
            ADIO_IreadContig(fh, buf, count, datatype,
                    ADIO_EXPLICIT_OFFSET, off, request,
                    &error_code);
        } else {
            /* to maintain strict atomicity semantics with other concurrent                                                                                                                                                
               operations, lock (exclusive) and call blocking routine */

            if (fh->file_system != ADIO_NFS) {
                ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);
            }

            ADIO_ReadContig(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                    off, &status, &error_code);

            if (fh->file_system != ADIO_NFS) {
                ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);
            }
            if (error_code == MPI_SUCCESS) {
                nbytes = count * datatype_size;
            }
            MPIO_Completed_request_create(&fh, nbytes, &error_code,
                    request);
        }
    } else {
        ADIO_IreadStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                shared_fp, request, &error_code);
    }

    /* record the end time */
    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);
    return error_code;
}

void mpi_file_iread_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count, MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iread_shared(c_fh, buf, *count, c_datatype, &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

#endif

int MPI_File_iwrite_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
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
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_IWRITEAT;

    ret_val =
        PMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iwrite_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Fint *request, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iwrite_at(c_fh, *offset, buf, *count, c_datatype, 
            &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_iwrite(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Request * request)
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
    iorec->operation = MPI_IWRITE;

    ret_val = PMPI_File_iwrite(mpi_fh, buf, count, datatype, request);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_iwrite_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iwrite(c_fh, buf, *count, c_datatype, &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_iwrite_shared(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPIO_Request * request)
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
    iorec->operation = MPI_IWRITESH;
    /* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    ADIO_File fh;
    int datatype_size, incr;
    ADIO_Status status;
    ADIO_Offset off, shared_fp;
    static char myname[] = "MPI_FILE_IWRITE_SHARED";

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

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;
    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);
    if (error_code != MPI_SUCCESS) {
        /* note: ADIO_Get_shared_fp should have set up error code already? */
        MPIO_Err_return_file(fh, error_code);
    }

    /* contiguous or strided? */
    if (buftype_is_contig && filetype_is_contig) {
        /* convert sizes to bytes */
        bufsize = datatype_size * count;
        off = fh->disp + fh->etype_size * shared_fp;
        if (!(fh->atomicity))
            ADIO_IwriteContig(fh, buf, count, datatype,
                    ADIO_EXPLICIT_OFFSET, off, request,
                    &error_code);
        else {
            /* to maintain strict atomicity semantics with other concurrent                                                                                                                                                
               operations, lock (exclusive) and call blocking routine */


            if (fh->file_system != ADIO_NFS)
                ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);

            ADIO_WriteContig(fh, buf, count, datatype,
                    ADIO_EXPLICIT_OFFSET, off, &status,
                    &error_code);

            if (fh->file_system != ADIO_NFS)
                ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);

            MPIO_Completed_request_create(&fh, bufsize, &error_code,
                    request);
        }
    } else
        ADIO_IwriteStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                shared_fp, request, &error_code);

    /* record the end time */
    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    return error_code;
}

void mpi_file_iwrite_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Fint *request, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    MPI_Request c_request;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_iwrite_shared(c_fh, buf, *count, c_datatype,
            &c_request);

    if(ret_val == MPI_SUCCESS)
        *request = MPI_Request_c2f(c_request);
    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_read_all(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Status * status)
{
    int ret_val;
    struct timeval start, end;
    int dtsize;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_READALL;

    ret_val = PMPI_File_read_all(mpi_fh, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

mpi_file_read_all_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read_all(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_read_at_all(MPI_File mpi_fh, MPI_Offset offset, void *buf,
        int count, MPI_Datatype datatype,
        MPI_Status * status)
{
    int ret_val;
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_READATALL;

    ret_val =
        PMPI_File_read_at_all(mpi_fh, offset, buf, count, datatype,
                status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_read_at_all_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read_at_all(c_fh, *offset, buf,
            *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_read_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
        int count, MPI_Datatype datatype, MPI_Status * status)
{
    int ret_val;
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_READAT;

    ret_val =
        PMPI_File_read_at(mpi_fh, offset, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_read_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read_at(c_fh, *offset, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_read(MPI_File mpi_fh, void *buf, int count,
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
    iorec->operation = MPI_READ;

    ret_val = PMPI_File_read(mpi_fh, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_read_(MPI_Fint *fh, void *buf, int *count,  MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    int ret_val;
    MPI_File c_fh;
    MPI_Datatype c_datatype;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_read(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

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

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

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

int MPI_File_read_shared(MPI_File mpi_fh, void *buf, int count,
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
    iorec->operation = MPI_READSH;
    /* end of add. by huaiming */

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    static char myname[] = "MPI_FILE_READ_SHARED";
    int datatype_size, incr;
    ADIO_Offset off, shared_fp;
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
    MPIO_CHECK_COUNT_SIZE(fh, count, datatype_size, myname, error_code);
    /* --END ERROR HANDLING-- */

    if (count * datatype_size == 0) {
#ifdef HAVE_STATUS_SET_BYTES
        MPIR_Status_set_bytes(status, datatype, 0);
#endif
        error_code = MPI_SUCCESS;
        goto fn_exit;
    }


    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_INTEGRAL_ETYPE(fh, count, datatype_size, myname,
            error_code);
    MPIO_CHECK_READABLE(fh, myname, error_code);
    MPIO_CHECK_FS_SUPPORTS_SHARED(fh, myname, error_code);
    /* --END ERROR HANDLING-- */

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;

    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);
    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
        error_code = MPIO_Err_return_file(fh, error_code);
        goto fn_exit;
    }
    /* --END ERROR HANDLING-- */

    /* contiguous or strided? */
    if (buftype_is_contig && filetype_is_contig) {
        /* convert count and shared_fp to bytes */
        bufsize = datatype_size * count;
        off = fh->disp + fh->etype_size * shared_fp;

        /* if atomic mode requested, lock (exclusive) the region, because there                                                                                                                                            
           could be a concurrent noncontiguous request. On NFS, locking                                                                                                                                                    
           is done in the ADIO_ReadContig. */

        if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
            ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);

        ADIO_ReadContig(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                off, status, &error_code);

        if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
            ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);
    } else {
        ADIO_ReadStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                shared_fp, status, &error_code);
        /* For strided and atomic mode, locking is done in ADIO_ReadStrided */
    }

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    return error_code;
}

void mpi_file_read_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr){
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val - MPI_File_read_shared(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

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

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_seek_(MPI_Fint *fh, MPI_Offset *offset, MPI_Fint *whence, MPI_Fint *ierr) {
    MPI_File c_fh;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);

    ret_val = MPI_File_seek(c_fh, *offset, *whence);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_write_all(MPI_File mpi_fh, void *buf, int count,
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
    iorec->operation = MPI_WRITEALL;

    ret_val = PMPI_File_write_all(mpi_fh, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_write_all_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_all(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_write_at_all(MPI_File mpi_fh, MPI_Offset offset, void *buf,
        int count, MPI_Datatype datatype,
        MPI_Status * status)
{
    int ret_val;
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_WRITEALL;

    ret_val =
        PMPI_File_write_at_all(mpi_fh, offset, buf, count, datatype,
                status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_write_at_all_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Status *status, MPI_Fint *ierr ) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_at_all(c_fh, *offset, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_write_at(MPI_File mpi_fh, MPI_Offset offset, void *buf,
        int count, MPI_Datatype datatype,
        MPI_Status * status)
{
    int ret_val;
    int dtsize;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    iorec->filedes = mpi_fh->fd_sys;
    //iorec->file_pos = mpi_fh->fp_ind;
    MPI_Type_size(datatype, &dtsize);
    iorec->file_pos = offset * dtsize;
    iorec->data_size = count * dtsize;
    iorec->op_time = start;
    iorec->operation = MPI_WRITEAT;

    ret_val =
        PMPI_File_write_at(mpi_fh, offset, buf, count, datatype, status);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return ret_val;
}

void mpi_file_write_at_(MPI_Fint *fh, MPI_Offset *offset, void *buf,
        MPI_Fint *count, MPI_Fint *datatype,
        MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_at(c_fh, *offset, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

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

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

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

int MPI_File_write_ordered(MPI_File mpi_fh, void *buf, int count,
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
    iorec->operation = MPI_WRITEORD;
    /* end of add. by huaiming */

    int error_code, datatype_size, nprocs, myrank, incr;
    int source, dest;
    static char myname[] = "MPI_FILE_WRITE_ORDERED";
    ADIO_Offset shared_fp;
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

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

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
        error_code = MPIO_Err_create_code(MPI_SUCCESS, MPIR_ERR_FATAL,
                myname, __LINE__, MPI_ERR_INTERN,
                "**iosharedfailed", 0);
        error_code = MPIO_Err_return_file(fh, error_code);
        goto fn_exit;
    }
    /* --END ERROR HANDLING-- */

    MPI_Send(NULL, 0, MPI_BYTE, dest, 0, fh->comm);

    ADIO_WriteStridedColl(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
            shared_fp, status, &error_code);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    /* FIXME: Check for error code from WriteStridedColl? */
    return error_code;
}

void mpi_file_write_ordered_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_ordered(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}

int MPI_File_write_shared(MPI_File mpi_fh, void *buf, int count,
        MPI_Datatype datatype, MPI_Status * status)
{

    /* add by huaiming*/
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
    iorec->operation = MPI_WRITESH;
    /* end of add, by huaiming*/

    int error_code, bufsize, buftype_is_contig, filetype_is_contig;
    static char myname[] = "MPI_FILE_READ_SHARED";
    int datatype_size, incr;
    ADIO_Offset off, shared_fp;
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
    MPIO_CHECK_COUNT_SIZE(fh, count, datatype_size, myname, error_code);
    /* --END ERROR HANDLING-- */

    if (count * datatype_size == 0) {
#ifdef HAVE_STATUS_SET_BYTES
        MPIR_Status_set_bytes(status, datatype, 0);
#endif
        error_code = MPI_SUCCESS;
        goto fn_exit;
    }

    /* --BEGIN ERROR HANDLING-- */
    MPIO_CHECK_INTEGRAL_ETYPE(fh, count, datatype_size, myname,
            error_code);
    MPIO_CHECK_FS_SUPPORTS_SHARED(fh, myname, error_code);
    /* --END ERROR HANDLING-- */

    ADIOI_Datatype_iscontig(datatype, &buftype_is_contig);
    ADIOI_Datatype_iscontig(fh->filetype, &filetype_is_contig);

    ADIOI_TEST_DEFERRED(fh, myname, &error_code);

    incr = (count * datatype_size) / fh->etype_size;

    ADIO_Get_shared_fp(fh, incr, &shared_fp, &error_code);
    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS) {
        error_code = MPIO_Err_create_code(MPI_SUCCESS, MPIR_ERR_FATAL,
                myname, __LINE__, MPI_ERR_INTERN,
                "**iosharedfailed", 0);
        error_code = MPIO_Err_return_file(fh, error_code);
        goto fn_exit;
    }
    /* --END ERROR HANDLING-- */

    if (buftype_is_contig && filetype_is_contig) {
        /* convert bufocunt and shared_fp to bytes */
        bufsize = datatype_size * count;
        off = fh->disp + fh->etype_size * shared_fp;

        /* if atomic mode requested, lock (exclusive) the region, because there                                                                                                                                            
           could be a concurrent noncontiguous request. On NFS, locking is                                                                                                                                                 
           done in the ADIO_WriteContig. */

        if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
            ADIOI_WRITE_LOCK(fh, off, SEEK_SET, bufsize);

        ADIO_WriteContig(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                off, status, &error_code);

        if ((fh->atomicity) && (fh->file_system != ADIO_NFS))
            ADIOI_UNLOCK(fh, off, SEEK_SET, bufsize);
    } else {
        ADIO_WriteStrided(fh, buf, count, datatype, ADIO_EXPLICIT_OFFSET,
                shared_fp, status, &error_code);
        /* For strided and atomic mode, locking is done in ADIO_WriteStrided */
    }

    /* --BEGIN ERROR HANDLING-- */
    if (error_code != MPI_SUCCESS)
        error_code = MPIO_Err_return_file(fh, error_code);
    /* --END ERROR HANDLING-- */

fn_exit:
    MPIR_Nest_decr();
    MPIU_THREAD_CS_EXIT(ALLFUNC,);

    gettimeofday(&end, NULL);
    iorec->op_end_time = end;

    IOSIG_mpiio_write_log(iorec);
    //PushIO_RTB_log(thisrank, iorec);

    return error_code;
}

void mpi_file_write_shared_(MPI_Fint *fh, void *buf, MPI_Fint *count,
        MPI_Fint *datatype, MPI_Status *status, MPI_Fint *ierr) {
    MPI_File c_fh;
    MPI_Datatype c_datatype;
    int ret_val;

    c_fh = MPI_File_f2c(*fh);
    c_datatype = MPI_Type_f2c(*datatype);

    ret_val = MPI_File_write_shared(c_fh, buf, *count, c_datatype, status);

    *ierr = (MPI_Fint)ret_val;
}
