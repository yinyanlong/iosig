#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include "iosig_trace.h"

#define EXE_TRACE_DEPTH 2

struct timeval bigbang = (struct timeval){0};
int my_rank = -1;
int current_depth = 0;

/*
 * Initial the log file for exe_prof.
 * The IO trace file is initialized in MPI_Init
 * by calling init_log();
 */
void __attribute__ ((constructor)) trace_begin (void) {
    char trace_file_path[FILE_PATH_LENGTH];
    char exe_logtext[80];

    gettimeofday(&bigbang, NULL);
    current_depth = 0;

    /* Initialize job_id and IOSIG_DATA_PATH */
    global_init();

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
        rename(old_file_path, new_file_path);
        /* No need to chech whether rename is done successfully. */
        
        /* Rename POSIX traces */
        get_trace_file_path_pid(old_file_path, TRACE_TYPE_POSIX);
        get_trace_file_path_rank(new_file_path, TRACE_TYPE_POSIX);
        rename(old_file_path, new_file_path);
    }
    
    /* TODO: translate the EXE traces to readable format */
    /* TODO: on exit() being called, this function will not be called.
     * Need to record the mapping between rank and pid 
     */
}

void __cyg_profile_func_enter (void *func, void *caller) {
//#ifdef EXE_TRACE
    ++current_depth;
    if (current_depth > EXE_TRACE_DEPTH) {
        return;
    }

    char exe_logtext[80];
    if(exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);
        sprintf(exe_logtext, "e %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
        __real_fwrite(exe_logtext, strlen(exe_logtext), 1, exe_fp);
    }
//#endif
}

void __cyg_profile_func_exit (void *func, void *caller) {
//#ifdef EXE_TRACE
    /* TODO: when exit(0) is called, the program exits immediately,
     * this tracing function will not be called
     */
    if (current_depth > EXE_TRACE_DEPTH) {
        --current_depth;
        return;
    }
    --current_depth;

    char exe_logtext[80];
    if (exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);

        sprintf(exe_logtext, "x %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
        __real_fwrite(exe_logtext, strlen(exe_logtext), 1, exe_fp);
    }
//#endif
}

