#include <stdio.h>
#include <sys/time.h>
#include "iosig_trace.h"

struct timeval bigbang = (struct timeval){0};
int my_rank = -1;

/*
 * Initial the log file for exe_prof.
 * The IO trace file is initialized in MPI_Init
 * by calling init_log();
 */
void __attribute__ ((constructor)) trace_begin (void) {
    char trace_file_path[FILE_PATH_LENGTH];

    /* initialize execution trace file */
    get_trace_file_path_pid(trace_file_path, TRACE_TYPE_EXE);
    exe_fp = fopen(trace_file_path, "a");

    /* initialize POSIX I/O trace file */
    get_trace_file_path_pid(trace_file_path, TRACE_TYPE_POSIX);
    posix_fp = fopen(trace_file_path, "a");
    
    gettimeofday(&bigbang, NULL);
}

void __attribute__ ((destructor)) trace_end (void) {
    /* close the trace files */
    if(exe_fp != NULL) {
        fclose(exe_fp);
    }
    if(posix_fp != NULL) {
        fclose(posix_fp);
    }

    /* if MPI are used, rename the pid based files to rank based */
    if(my_rank != -1) {
        int pid = getpid();
        char old_file_path[FILE_PATH_LENGTH];
        char new_file_path[FILE_PATH_LENGTH];
        /* Rename EXE traces */
        get_trace_file_path_pid(old_file_path, TRACE_TYPE_EXE);
        get_trace_file_path_rank(new_file_path, TRACE_TYPE_EXE);
        if(rename(old_file_path, new_file_path) == 0) {
            printf("%s has been rename %s.\n", old_file_path, new_file_path);
        } else {
            fprintf(stderr, "Error renaming %s.\n", old_file_path);
        }
        
        /* Rename POSIX traces */
        get_trace_file_path_pid(old_file_path, TRACE_TYPE_POSIX);
        get_trace_file_path_rank(new_file_path, TRACE_TYPE_POSIX);
        if(rename(old_file_path, new_file_path) == 0) {
            printf("%s has been rename %s.\n", old_file_path, new_file_path);
        } else {
            fprintf(stderr, "Error renaming %s.\n", old_file_path);
        }
    }
}

void __cyg_profile_func_enter (void *func, void *caller) {
    if(exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);
        fprintf(exe_fp, "e %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
    }
}

void __cyg_profile_func_exit (void *func, void *caller) {
    if (exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);

        fprintf(exe_fp, "x %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
    }
}

