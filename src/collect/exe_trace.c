#include <stdio.h>
#include <sys/time.h>
#include <string.h>
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
    char exe_logtext[80];

    /* initialize execution trace file */
    get_trace_file_path_pid(trace_file_path, TRACE_TYPE_EXE);
    exe_fp = __real_fopen64((char*)trace_file_path, (char*)"a");

    /* initialize POSIX I/O trace file */
    get_trace_file_path_pid(trace_file_path, TRACE_TYPE_POSIX);
    posix_fp = __real_fopen64((char*)trace_file_path, (char*)"a");
    sprintf(exe_logtext, "# OPER  FH  POS  SIZE  START  END  PATH\n");
    __real_fwrite(exe_logtext, strlen(exe_logtext), 1, posix_fp);
    sprintf(exe_logtext, "#--------------------------------------\n");
    __real_fwrite(exe_logtext, strlen(exe_logtext), 1, posix_fp);
    //fprintf(posix_fp, "# OPER  FH  POS  SIZE  START  END  PATH\n");
    //fprintf(posix_fp, "#--------------------------------------\n");
    
    gettimeofday(&bigbang, NULL);
}

void __attribute__ ((destructor)) trace_end (void) {
    /* close the trace files */
    if(exe_fp != NULL) {
        __real_fclose(exe_fp);
    }
    if(posix_fp != NULL) {
        __real_fclose(posix_fp);
    }

    /* if MPI are used, rename the pid based files to rank based */
    if(my_rank != -1) {
        int pid = getpid();
        char old_file_path[FILE_PATH_LENGTH];
        char new_file_path[FILE_PATH_LENGTH];
        /* Rename EXE traces */
        get_trace_file_path_pid(old_file_path, TRACE_TYPE_EXE);
        get_trace_file_path_rank(new_file_path, TRACE_TYPE_EXE);
        if(rename(old_file_path, new_file_path) != 0) {
            printf("Error renaming %s.\n", old_file_path);
        }
        
        /* Rename POSIX traces */
        get_trace_file_path_pid(old_file_path, TRACE_TYPE_POSIX);
        get_trace_file_path_rank(new_file_path, TRACE_TYPE_POSIX);
        if(rename(old_file_path, new_file_path) != 0) {
            printf("Error renaming %s.\n", old_file_path);
        }
    }
    
    /* TODO: translate the EXE traces to readable format */
}

void __cyg_profile_func_enter (void *func, void *caller) {
    char exe_logtext[80];
    if(exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);
        sprintf(exe_logtext, "e %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
        __real_fwrite(exe_logtext, strlen(exe_logtext), 1, exe_fp);
    }
}

void __cyg_profile_func_exit (void *func, void *caller) {
    char exe_logtext[80];
    if (exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);

        sprintf(exe_logtext, "x %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
        __real_fwrite(exe_logtext, strlen(exe_logtext), 1, exe_fp);
    }
}

