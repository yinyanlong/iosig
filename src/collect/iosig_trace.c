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
 *              03/29/2007 by Suren Byna Added RTB, a linked list implementation
 *                                       of buffer to hold traces. The size of
 *                                       RTB is 1024 for each process. When the
 *                                       limit is reached, overflowing traces are
 *                                       kept in a file corresponding to that process.
 *              09/14/2011 by Yanlong Yin Added the end time of each operation to trace.
 *
 * Funded by:   NSF, Award # CCF-0621435
 *
 * File name: pushio_trace.c
 * Purpose  : Contains tracing functions.
 *	      Currently all traces are written into a file. 
 *	      In future, these traces have to be written into a buffer
 *	      that can be read by another thread, and analize for patterns.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>
#include "iosig_trace.h"


/********************
  Logging functions, using a file. These functions will be removed and replaced by RTB
  related functions, ultimately.
 ********************/

/* Initialize log file  */
void init_log(int rank) {
    char filename_rank[25];
    sprintf(filename_rank, "mpiio_trace_rank-%d.out", rank);

    out_fp = fopen(filename_rank, "a");
    if (!out_fp) {
        fprintf(stderr,
                "Pointer to log file doesn't exist in init_log \n");
        exit(1);
    } else {
        fprintf(out_fp,
                "\nProc ID  MPI_Rank   File #     File Pos \t # of Bytes \t Time(s)     I/O Op   EndTime(s)\n");
        fprintf(out_fp,
                "===================================================================================== \n");
    }
    return;
}

/* close log files */
void end_log()
{
    if (out_fp) {
        fprintf(out_fp, "\nEnd of trace \n");
        fclose(out_fp);
    }
    return;
}

/* write log information into a file  */
void log_read_trace(PushIO_Trace_record * pushio_rec)
{
    struct timeval diffstart, diffend;
    char operation[15];
    int rank;

    /* find MPI_rank information  */
    if (!pushio_rec->is_mpi_operation) {
        rank = -1;
    } else {
        rank = pushio_rec->mpi_rank;
    }

    if (!out_fp) {
        fprintf(stderr,
                "Pointer to log file doesn't exist in log_read_trace. Opening now. \n");

        /* TODO: consider removing the following 3 lines */
        char filename_rank[25];
        sprintf(filename_rank, "mpiio_trace_rank-%d.out", rank);
        out_fp = fopen(filename_rank, "a");
    }
    timeval_diff(&diffstart, &(pushio_rec->op_time), &bigbang);
    timeval_diff(&diffend, &(pushio_rec->op_end_time), &bigbang);

    get_operation(operation, pushio_rec->operation);

    fprintf(out_fp, "%d %6d %6d  %12ld  %12ld    %6ld.%06ld  %20s   %6ld.%06ld\n",
            getpid(), rank, pushio_rec->filedes, pushio_rec->file_pos,
            pushio_rec->data_size, (long) diffstart.tv_sec,
            (long) diffstart.tv_usec, operation, (long)diffend.tv_sec,
            (long) diffend.tv_usec);
    return;
}

/********** RTB related functions  *************************/

/* log trace records into a buffer called RTB.   */

void PushIO_RTB_log(int rank, PushIO_Trace_record * pushio_rec)
{
    PushIO_RTB_node *rtb_node;
    PushIO_RTB_node *temp_node;
    char operation[15];
    struct timeval difftv;

    rtb_node = (PushIO_RTB_node *) malloc(sizeof(PushIO_RTB_node));

    rtb_node->rtb_entry.is_mpi_operation = pushio_rec->is_mpi_operation;

    /* find MPI_rank information  */
    if (!pushio_rec->is_mpi_operation) {
        rtb_node->rtb_entry.mpi_rank = -1;
    } else {
        rtb_node->rtb_entry.mpi_rank = pushio_rec->mpi_rank;
    }

    switch (pushio_rec->operation) {
        case (MPI_READ):
        case (MPI_OPEN):
        case (MPI_CLOSE):
        case (MPI_IREAD):
        case (MPI_IREADAT):
        case (MPI_READAT):
        case (MPI_READATALL):
        case (MPI_READALL):
        case (MPI_WRITE):
        case (MPI_IWRITE):
        case (MPI_IWRITEAT):
        case (MPI_WRITEAT):
        case (MPI_WRITEATALL):
        case (MPI_WRITEALL):
        case (MPI_SEEK):
        case (MPI_READSH):
        case (MPI_IREADSH):
        case (MPI_READORD):
        case (MPI_WRITESH):
        case (MPI_IWRITESH):
        case (MPI_WRITEORD):
        case (POSIX_READ):
        case (POSIX_WRITE):
        case (POSIX_SEEK):
            rtb_node->rtb_entry.operation = pushio_rec->operation;
            break;
        default:
            rtb_node->rtb_entry.operation = -100;
            break;
    }

    timeval_diff(&difftv, &(pushio_rec->op_time), &bigbang);

    rtb_node->rtb_entry.filedes = pushio_rec->filedes;
    rtb_node->rtb_entry.file_pos = pushio_rec->file_pos;
    rtb_node->rtb_entry.data_size = pushio_rec->data_size;
    rtb_node->rtb_entry.op_time = difftv;

    get_operation(operation, pushio_rec->operation);

    /* add rtb_node to the buffer related to this rank  */

    if (iortb_head == NULL) {
        num_entries = 1;
        rtb_node->prev_rtb_node = NULL;
        rtb_node->next_rtb_node = NULL;
        iortb_head = rtb_node;
        iortb_tail = rtb_node;
    } else {
        ++num_entries;
        if (num_entries > MAX_NUM_RTB_ENTRIES) {
            /* Put the head record into a file and get a new head */
            PushIO_RTB_backup(rank, iortb_head->rtb_entry);
            --num_entries;
            temp_node = iortb_head;

            iortb_head = iortb_head->next_rtb_node;
            iortb_head->prev_rtb_node = NULL;

            free(temp_node);
        }
        rtb_node->prev_rtb_node = iortb_tail;
        iortb_tail->next_rtb_node = rtb_node;
        rtb_node->next_rtb_node = NULL;
        iortb_tail = rtb_node;
    }
    return;
}

/*  Initialize request tracing using RTBs */
int PushIO_RTB_init(int rank)
{
    char filename_rank[15];

    iortb_head = (PushIO_RTB_node *) NULL;
    iortb_tail = (PushIO_RTB_node *) NULL;

    sprintf(filename_rank, "trace_r%d.out", rank);

    fp_rank = fopen(filename_rank, "a");
    if (!fp_rank) {
        fprintf(stderr,
                "Pointer to log file doesn't exist in init_log \n");
        PushIO_RTB_finalize(rank);
        exit(1);
    } else {
        fprintf(fp_rank,
                "\nMPI_Rank   File #     File Pos \t # of Bytes \t Time(s)     I/O Op\n");
        fprintf(fp_rank,
                "===================================================================================== \n");
    }
    return 1;
}

/*  Finalize request tracing using RTBs */

int PushIO_RTB_finalize(int rank)
{
    if (iortb_head == NULL) {
        return 1;
    } else {
        PushIO_RTB_file_traces(rank);	/* We can just discard the last batch of traces  */
        iortb_head = NULL;
        iortb_tail = NULL;
    }
    if (fp_rank) {
        fclose(fp_rank);
    }
    return 1;
}

/* backup trace records into a file when an RTB is full  */

int PushIO_RTB_backup(int rank, PushIO_Trace_record pushio_rec)
{
    char operation[15];

    if (!fp_rank) {
        fprintf(stderr,
                "Pointer to log file doesn't exist in init_log \n");
        return -1;
    }

    get_operation(operation, pushio_rec.operation);

    fprintf(fp_rank, "%6d %6d  %12ld  %12ld %6ld.%06ld  %s\n",
            pushio_rec.mpi_rank,
            pushio_rec.filedes,
            pushio_rec.file_pos,
            pushio_rec.data_size,
            (long) pushio_rec.op_time.tv_sec,
            (long) pushio_rec.op_time.tv_usec, operation);
    return 1;
}

/* write all the remaining traces into files related to processes, if needed.  */

int PushIO_RTB_file_traces(int rank)
{
    PushIO_RTB_node *rtb_node;
    char operation[15];

    /* rtb_node = (PushIO_RTB_node *) malloc (sizeof (PushIO_RTB_node)); */
    rtb_node = iortb_head;

    do {
        get_operation(operation, rtb_node->rtb_entry.operation);

        fprintf(fp_rank, "%6d %6d  %12ld  %12ld %6ld.%06ld  %s\n",
                rtb_node->rtb_entry.mpi_rank,
                rtb_node->rtb_entry.filedes,
                rtb_node->rtb_entry.file_pos,
                rtb_node->rtb_entry.data_size,
                (long) rtb_node->rtb_entry.op_time.tv_sec,
                (long) rtb_node->rtb_entry.op_time.tv_usec, operation);
        rtb_node = rtb_node->next_rtb_node;
    } while (rtb_node != NULL);

    /* free (rtb_node); */
    return 1;
}

/* dump all the PushIO_Trace_Records from RTBs into log files */
/* One log file is maintained for each process  */

int PushIO_RTB_dump_trace(int rank)
{
    char filename_rank[15];
    FILE *fp_rank;
    PushIO_RTB_node *rtb_node;
    char operation[15];

    sprintf(filename_rank, "trace_%d.out", rank);

    fp_rank = fopen(filename_rank, "w");
    if (!fp_rank) {
        fprintf(stderr,
                "Pointer to log file doesn't exist in init_log \n");
        PushIO_RTB_finalize(rank);
        exit(1);
    } else {
        fprintf(fp_rank,
                "\nMPI_Rank   File #     File Pos \t # of Bytes \t Time(s)     I/O Op\n");
        fprintf(fp_rank,
                "===================================================================================== \n");
    }

    rtb_node = (PushIO_RTB_node *) malloc(sizeof(PushIO_RTB_node));
    rtb_node = iortb_head;

    do {
        get_operation(operation, rtb_node->rtb_entry.operation);

        fprintf(fp_rank, "%6d %6d  %12ld  %12ld %6ld.%06ld  %s\n",
                rtb_node->rtb_entry.mpi_rank,
                rtb_node->rtb_entry.filedes,
                rtb_node->rtb_entry.file_pos,
                rtb_node->rtb_entry.data_size,
                (long) rtb_node->rtb_entry.op_time.tv_sec,
                (long) rtb_node->rtb_entry.op_time.tv_usec, operation);
        rtb_node = rtb_node->next_rtb_node;
    } while (rtb_node != NULL);

    free(rtb_node);
    fclose(fp_rank);
    return;
}

/* Remove a trace record from an RTB. Currently not used  */

int PushIO_RTB_remove(int rank, PushIO_RTB_node * rtb_node)
{
    fprintf(stderr,
            "A trace is removed from PushIO_RTB for process id: %d \n",
            getpid());

    return 1;
}


/********   Utility functions    *******/

/* get time interval between two timeval structures  */

int timeval_diff(struct timeval *result, struct timeval *x,
        struct timeval *y)
{
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
       tv_usec is certainly positive. */
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;

    /* Return 1 if result is negative. */
    return x->tv_sec < y->tv_sec;
}


/* get a string for MPI operation from operation codes  */

void get_operation(char *operation, int rec_operation)
{
    switch (rec_operation) {
        case (MPI_READ):
            sprintf(operation, "MPI_READ");
            break;
        case (MPI_IREAD):
            sprintf(operation, "MPI_IREAD");
            break;
        case (MPI_IREADAT):
            sprintf(operation, "MPI_IREADAT");
            break;
        case (MPI_READAT):
            sprintf(operation, "MPI_READAT");
            break;
        case (MPI_READATALL):
            sprintf(operation, "MPI_READATALL");
            break;
        case (MPI_READALL):
            sprintf(operation, "MPI_READALL");
            break;
        case (MPI_WRITE):
            sprintf(operation, "MPI_WRITE");
            break;
        case (MPI_IWRITE):
            sprintf(operation, "MPI_IWRITE");
            break;
        case (MPI_IWRITEAT):
            sprintf(operation, "MPI_IWRITEAT");
            break;
        case (MPI_WRITEAT):
            sprintf(operation, "MPI_WRITEAT");
            break;
        case (MPI_WRITEATALL):
            sprintf(operation, "MPI_WRITEATALL");
            break;
        case (MPI_WRITEALL):
            sprintf(operation, "MPI_WRITEALL");
            break;
        case (MPI_SEEK):
            sprintf(operation, "MPI_SEEK");
            break;
        case (MPI_OPEN):
            sprintf(operation, "MPI_OPEN");
            break;
        case (MPI_CLOSE):
            sprintf(operation, "MPI_CLOSE");
            break;
        case (MPI_READSH):
            sprintf(operation, "MPI_READSH");
            break;
        case (MPI_IREADSH):
            sprintf(operation, "MPI_IREADSH");
            break;
        case (MPI_READORD):
            sprintf(operation, "MPI_READORD");
            break;
        case (MPI_WRITESH):
            sprintf(operation, "MPI_WRITESH");
            break;
        case (MPI_IWRITESH):
            sprintf(operation, "MPI_IWRITESH");
            break;
        case (MPI_WRITEORD):
            sprintf(operation, "MPI_WRITEORD");
            break;
        case (POSIX_SEEK):
            sprintf(operation, "POSIX_SEEK");
            break;
        case (POSIX_READ):
            sprintf(operation, "POSIX_READ");
            break;
        case (POSIX_WRITE):
            sprintf(operation, "POSIX_WRITE");
            break;
        case (POSIX_OPEN):
            sprintf(operation, "POSIX_WRITE");
            break;
        case (POSIX_CLOSE):
            sprintf(operation, "POSIX_WRITE");
            break;
        default:
            sprintf(operation, "NOT KNOWN");
            break;
    }
}

void getProcCmdLine (int *ac, char **av)
{
    int i = 0, pid;
    char *inbuf, file[256];
    FILE *infile;
    char *arg_ptr;

    *ac = 0;
    *av = NULL;

    pid = getpid ();
    snprintf (file, 256, "/proc/%d/cmdline", pid);
    infile = fopen (file, "r");

    if (infile != NULL)
    {
        while (!feof (infile))
        {
            inbuf = malloc (IOSIG_MAX_ARG_STRING_SIZE);
            if (fread (inbuf, 1, IOSIG_MAX_ARG_STRING_SIZE, infile) > 0)
            {
                arg_ptr = inbuf;
                while (*arg_ptr != '\0')
                {
                    av[i] = strdup (arg_ptr);
                    arg_ptr += strlen (av[i]) + 1;
                    i++;
                }
            }
            free (inbuf);
        }
        *ac = i;

        fclose (infile);
    }
}

void getProcCmdLine2 (int *ac, char **av)
{
    int i = 0, pid;
    int length = 0;
    char *inbuf;
    char file[256];
    FILE *infile;
    char *arg_ptr;

    *ac = 0;
    *av = NULL;

    pid = getpid ();
    snprintf (file, 256, "/proc/%d/cmdline", pid);
    infile = fopen (file, "r");

    if (infile != NULL)
    {
        //while (!feof (infile))
        if (!feof (infile))
        {
            //inbuf = malloc (IOSIG_MAX_ARG_STRING_SIZE);
            av[0] = malloc (IOSIG_MAX_ARG_STRING_SIZE);
            memset(av[0], 0, IOSIG_MAX_ARG_STRING_SIZE);
            //av[0] = inbuf;
            length = fread (av[0], 1, IOSIG_MAX_ARG_STRING_SIZE, infile);

            if ( length > 0)
            {
                // no need to loop to replace whitespace to '\0'
                // just assign pointers to head of each arg
                arg_ptr = av[0];
                while (*arg_ptr != '\0')
                {
                    if (i!=0)
                        av[i] = arg_ptr;
                    if ( (arg_ptr+strlen(av[i])+1) < av[0]+IOSIG_MAX_ARG_STRING_SIZE )
                        arg_ptr += strlen (av[i]) + 1;
                    i++;
                }
            }
            //free (inbuf);
        }
        *ac = i;

        fclose (infile);
    }
}


void get_trace_file_path_pid(char *path, int trace_type) {
    int pid = getpid();

    switch (trace_type) {
        case TRACE_TYPE_EXE:
            sprintf(path, "exe_trace_pid-%d.out", pid);
            break;
        case TRACE_TYPE_MPIIO:
            sprintf(path, "mpiio_trace_pid-%d.out", pid);
            break;
        case TRACE_TYPE_POSIX:
            sprintf(path, "posix_trace_pid-%d.out", pid);
            break;
    }
}

void get_trace_file_path_rank(char *path, int trace_type) {
    int pid = getpid();
    if (my_rank == -1) {
        get_trace_file_path_pid(path, trace_type);
        return;
    }

    switch (trace_type) {
        case TRACE_TYPE_EXE:
            sprintf(path, "exe_trace_rank-%d.out", my_rank);
            break;
        case TRACE_TYPE_MPIIO:
            sprintf(path, "mpiio_trace_rank-%d.out", my_rank);
            break;
        case TRACE_TYPE_POSIX:
            sprintf(path, "posix_trace_rank-%d.out", my_rank);
            break;
    }
}

