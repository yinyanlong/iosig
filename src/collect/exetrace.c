#include <stdio.h>
#include <sys/time.h>
#include "iosig_trace.h"

//static FILE *exe_fp;
//static struct timeval bigbang;
//static unsigned long bigbang;

/*
 * Initial the log file for exe_prof.
 * The IO trace file is initialized in MPI_Init
 * by calling init_log();
 */
void
__attribute__ ((constructor))
trace_begin (void)
{
    char exefilename_pid[25];
    int pid = getpid();
    sprintf(exefilename_pid, "exe_%d.out", pid);
    exe_fp = fopen(exefilename_pid, "a");
    
    gettimeofday(&bigbang, NULL);
    //bigbang = tval.tv_sec*1000000+tval.tv_usec;
    //fprintf(exe_fp, "s 0x0 0x0 %lu\n", bigbang);
}

void
__attribute__ ((destructor))
trace_end (void)
{
    if(exe_fp != NULL) {
        fclose(exe_fp);
    }
}

void
__cyg_profile_func_enter (void *func, void *caller)
{
    if(exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);
        fprintf(exe_fp, "e %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
        //fprintf(exe_fp, "e %p %p %lu\n", func, caller, tval.tv_sec*1000000+tval.tv_usec);
//        fprintf(exe_fp, "e %p %p %lu\n", func, caller, time(NULL) );
    }
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
    if (exe_fp != NULL) {
        struct timeval current, difftime;
        gettimeofday(&current, NULL);
        timeval_diff(&difftime, &current, &bigbang);

        fprintf(exe_fp, "x %p %p %6ld.%06ld\n", func, caller, (long) difftime.tv_sec, (long) difftime.tv_usec );
//        fprintf(exe_fp, "x %p %p %lu\n", func, caller, tval.tv_sec*1000000+tval.tv_usec);
        //fprintf(exe_fp, "x %p %p %lu\n", func, caller, time(NULL));
    }
}

