#include <stdio.h>
#include "iosig_trace.h"

/* declare the real calls */
FILE* __real_fopen (const char *path, const char *mode);
int __real_fclose (FILE *stream);
size_t __real_fread (void * ptr, size_t size, size_t nitems, FILE * stream);
size_t __real_fwrite (const void * ptr, size_t size, size_t nitems, FILE * stream);
int __real_fseek (FILE *stream, long offset, int whence);

/* utility functions */
int IOSIG_init_posix_log() {
    /* seems nothing to do here.
     * all the initilization is put into the constructor 
     * function in exe_trace.c
     */
}

/* declare and define the wrap calls */
FILE* __wrap_fopen (const char *path, const char *mode) {
    FILE * ret;
    printf(">>>>>>>>>>>>>>>>>\n");
    ret =  __real_fopen(path, mode);
    return ret;
}

int __wrap_fclose (FILE* stream) {
    return 0;
}

size_t __wrap_fread(void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    return 0;
}

size_t __wrap_fwrite(const void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    return 0;
}

int __wrap_fseek(FILE *stream, long offset, int whence) {
    return 0;
}
