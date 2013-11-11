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

    printf("Before PMPI_File_open\n");
    ret_val = PMPI_File_open(comm, filename, amode, info, fh);
    printf("After PMPI_File_open\n");

    iorec->is_mpi_operation = 1;
    iorec->mpi_rank = thisrank;
    if (fh) {
        iorec->filedes = (*fh)->fd_sys;
    } else {
        iorec->filedes = 0;		/* FIXME  */
    }
    iorec->data_size = 0;
    iorec->op_time = start;
    iorec->operation = MPI_OPEN;
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

