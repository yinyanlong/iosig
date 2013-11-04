#ifndef PUSHIO_TRACE_H
#define PUSHIO_TRACE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>

#define MAX_NUM_PROCS 4
#define MAX_NUM_RTB_ENTRIES 1024
#define FILE_PATH_LENGTH 256
#define IOSIG_MAX_ARG_STRING_SIZE 4096

extern int my_rank;              /* my_rank is initialized in -1, a value larger
                                    than -1 means MPI are being used because it
                                    get updated in MPI_Init(). */
extern struct timeval bigbang;   /* bigbang means the beginning of everything,
                                    here it means the beginning of this 
                                    application */

FILE *out_fp; /* file pointer of the MPIIO trace file */
FILE *exe_fp; /* file pointer of the execution trace file */
FILE *posix_fp; /* file pointer of the POSIX I/O trace file */

FILE *fp_rank;

typedef struct iosig_mpiio_trace_record_t {
    int is_mpi_operation;
    int mpi_rank;
    int filedes;
    long int file_pos;
    size_t data_size;
    int operation;
    struct timeval op_time;
    struct timeval op_end_time;
} iosig_mpiio_trace_record;


char mpiio_logtext[512];    /* Pre define this variable here so the *_write_log()
                         do not need to create it each time  */
char posix_logtext[512];

iosig_mpiio_trace_record *iorec;   /* TODO: this is not thread safe.  */
int thisrank;

/******** Declare the real POSIX calls ********/
/*
 * POSIX Standard: 6.5 File Control Operations <fcntl.h>
 */
int __real_open(const char *path, int oflag, ... );
int __real_open64(const char *path, int oflag, ... );
/*
 * POSIX Standard: 2.10 Symbolic Constants     <unistd.h>
 */
int __real_close(int fildes);
ssize_t __real_read(int fildes, void *buf, size_t nbyte);
ssize_t __real_write(int fildes, const void *buf, size_t nbyte);
off_t __real_lseek(int fildes, off_t offset, int whence);
off_t __real_lseek64(int fildes, off_t offset, int whence);
ssize_t __real_pread(int fildes, void *buf, size_t nbyte, off_t offset);
ssize_t __real_pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
ssize_t __real_pread64(int fildes, void *buf, size_t nbyte, off64_t offset);
ssize_t __real_pwrite64(int fildes, const void *buf, size_t nbyte, off64_t offset);

/*
 * ISO C99 Standard: 7.19 Input/output <stdio.h>
 */
FILE* __real_fopen (const char *path, const char *mode);
FILE* __real_fopen64 (const char *path, const char *mode);
int __real_fclose (FILE *stream);
size_t __real_fread (void * ptr, size_t size, size_t nitems, FILE * stream);
size_t __real_fwrite (const void * ptr, size_t size, size_t nitems, FILE * stream);
int __real_fseek (FILE *stream, long offset, int whence);

void init_log(int rank);
void end_log();
void log_read_trace(iosig_mpiio_trace_record * pushio_rec);
int timeval_diff(struct timeval *result, struct timeval *x,
		 struct timeval *y);
void get_operation(char *operation, int rec_operation);
void get_trace_file_path_pid(char *path, int trace_type);
void get_trace_file_path_rank(char *path, int trace_type);

enum iosig_trace_type {
    TRACE_TYPE_EXE = 1,
    TRACE_TYPE_MPIIO,
    TRACE_TYPE_POSIX
};

enum iosig_io_operation {
    MPI_OPEN = 1,
    MPI_CLOSE,
    MPI_SEEK,
    MPI_READ,
    MPI_IREAD,
    MPI_READAT,
    MPI_READALL,
    MPI_READATALL,
    MPI_IREADAT,
    MPI_READSH,
    MPI_IREADSH,
    MPI_READORD,

    MPI_WRITE,
    MPI_IWRITE,
    MPI_WRITEAT,
    MPI_WRITEALL,
    MPI_WRITEATALL,
    MPI_IWRITEAT,
    MPI_WRITESH,
    MPI_IWRITESH,
    MPI_WRITEORD,

    POSIX_OPEN,
    POSIX_CLOSE,
    POSIX_READ,
    POSIX_WRITE,
    POSIX_LSEEK,

    POSIX_FOPEN,
    POSIX_FCLOSE,
    POSIX_FREAD,
    POSIX_FWRITE,
    POSIX_FSEEK
};
    

/* declare pointers to IO Request Trace Buffers.
   * One buffer for each process.
   * Implemented as doubly linked lists */

typedef struct PushIO_RTB_node_t {
    struct PushIO_RTB_node_t *prev_rtb_node;
    iosig_mpiio_trace_record rtb_entry;
    struct PushIO_RTB_node_t *next_rtb_node;
} PushIO_RTB_node;

PushIO_RTB_node *iortb_head;
PushIO_RTB_node *iortb_tail;
/* Maintain the number of entries in an RTB corresponding to a process  */
int num_entries;

/* PushIO RTB doubly linked list functions */

int PushIO_RTB_init(int rank);
void PushIO_RTB_log(int rank, iosig_mpiio_trace_record * pushio_rec);
int PushIO_RTB_backup(int rank, iosig_mpiio_trace_record pushio_rec);
int PushIO_RTB_remove(int rank, PushIO_RTB_node * rtb_node);
int PushIO_RTB_dump_trace(int rank);
int PushIO_RTB_file_traces(int rank);
int PushIO_RTB_finalize(int rank);

#endif

