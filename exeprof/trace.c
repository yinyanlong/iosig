#include <stdio.h>
#include <sys/time.h>

static FILE *fp_trace;
//static struct timeval bigbang;
static unsigned long bigbang;

void
__attribute__ ((constructor))
trace_begin (void)
{
    fp_trace = fopen("trace.out", "w");
    struct timeval tval;
    gettimeofday(&tval, NULL);
    bigbang = tval.tv_sec*1000000+tval.tv_usec;
    fprintf(fp_trace, "s 0x0 0x0 %lu\n", bigbang);
}

void
__attribute__ ((destructor))
trace_end (void)
{
    if(fp_trace != NULL) {
        fclose(fp_trace);
    }
}

void
__cyg_profile_func_enter (void *func, void *caller)
{
    if(fp_trace != NULL) {
        struct timeval tval;
        gettimeofday(&tval, NULL);
        fprintf(fp_trace, "e %p %p %lu\n", func, caller, tval.tv_sec*1000000+tval.tv_usec);
//        fprintf(fp_trace, "e %p %p %lu\n", func, caller, time(NULL) );
    }
}

void
__cyg_profile_func_exit (void *func, void *caller)
{
    if (fp_trace != NULL) {
        struct timeval tval;
        gettimeofday(&tval, NULL);
        fprintf(fp_trace, "x %p %p %lu\n", func, caller, tval.tv_sec*1000000+tval.tv_usec);
        //fprintf(fp_trace, "x %p %p %lu\n", func, caller, time(NULL));
    }
}

