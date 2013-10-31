/* This file is part of the Server-Push File Access Server (FAS) environment
 *
 *            <<<<  Add more info >>>
 ****************************************************************************
 *
 * Author:      Suren Byna (sbyna@iit.edu)
 *              Illinois Institute of Technology &
 *              Argonne National Laboratory
 * Created on:  03/04/2007
 * Modified on: 03/16/2007 by Suren Byna
 *		03/26/2007 by Suren Byna, to add RTB
 *              09/14/2011 by Yanlong Yin, to add op_end_time
 *
 * Funded by:   NSF, Award # CCF-0621435
 *
 * File name: pushio_trace.h
 * Purpose  : 
 *
 */

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

typedef struct PushIO_Trace_record_t {
    int is_mpi_operation;
    int mpi_rank;
    int filedes;
    long int file_pos;
    size_t data_size;
    int operation;
    struct timeval op_time;
    struct timeval op_end_time;
} PushIO_Trace_record;

/* declare pointers to IO Request Trace Buffers.
   * One buffer for each process.
   * Implemented as doubly linked lists */

typedef struct PushIO_RTB_node_t {
    struct PushIO_RTB_node_t *prev_rtb_node;
    PushIO_Trace_record rtb_entry;
    struct PushIO_RTB_node_t *next_rtb_node;
} PushIO_RTB_node;

PushIO_RTB_node *iortb_head;
PushIO_RTB_node *iortb_tail;

/* end of add: shm 2009.12 */
PushIO_Trace_record *iorec;
int thisrank;
/* end of add: shm 2009.12 */

/* Maintain the number of entries in an RTB corresponding to a process  */
int num_entries;

/* PushIO RTB doubly linked list functions */

int PushIO_RTB_init(int rank);
void PushIO_RTB_log(int rank, PushIO_Trace_record * pushio_rec);
int PushIO_RTB_backup(int rank, PushIO_Trace_record pushio_rec);
int PushIO_RTB_remove(int rank, PushIO_RTB_node * rtb_node);
int PushIO_RTB_dump_trace(int rank);
int PushIO_RTB_file_traces(int rank);
int PushIO_RTB_finalize(int rank);

void init_log(int rank);
void end_log();
void log_read_trace(PushIO_Trace_record * pushio_rec);
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
    POSIX_SEEK
};
    
#endif

