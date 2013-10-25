#include <stdio.h>
#include <stdarg.h>
#include "iosig_trace.h"

typedef struct IOSIG_POSIX_file_t {
    int file_handler;
    long int file_pos;
} IOSIG_POSIX_file;

/******** declare the real calls ********/
void IOSIG_trace_posix_open () {

}

/*
 *  POSIX Standard: 6.5 File Control Operations <fcntl.h>
 */
int __real_open(const char *path, int oflag, ... );
/*
 *  POSIX Standard: 2.10 Symbolic Constants     <unistd.h>
 */
int __real_close(int fildes);
ssize_t __real_read(int fildes, void *buf, size_t nbyte);
ssize_t __real_write(int fildes, const void *buf, size_t nbyte);
off_t __real_lseek(int fildes, off_t offset, int whence);

/*
 *  ISO C99 Standard: 7.19 Input/output <stdio.h>
 */
FILE* __real_fopen (const char *path, const char *mode);
int __real_fclose (FILE *stream);
size_t __real_fread (void * ptr, size_t size, size_t nitems, FILE * stream);
size_t __real_fwrite (const void * ptr, size_t size, size_t nitems, FILE * stream);
int __real_fseek (FILE *stream, long offset, int whence);

/* utility functions */

/******** the wrap calls ********/
int __wrap_open(const char *path, int oflag, ... ) {
    int mode;
    int ret_val;
    struct timeval start, end;

    /* check whether there is the 3rd arg */
    if (oflag & O_CREAT) {
        va_list mode_arg;
        va_start(mode_arg, oflag);
        mode = va_arg(mode_arg, int);
        va_end(mode_arg);

        gettimeofday(&start, NULL);
        ret_val = __real_open(path, oflag, mode);
        gettimeofday(&end, NULL);
    } else {
        gettimeofday(&start, NULL);
        ret_val = __real_open(path, oflag);
        gettimeofday(&end, NULL);
    }

    IOSIG_trace_posix_open(); /* TODO */
}

int __wrap_close(int fildes) {

}

ssize_t __wrap_read(int fildes, void *buf, size_t nbyte) {

}

ssize_t __wrap_write(int fildes, const void *buf, size_t nbyte) {

}

off_t __wrap_lseek(int fildes, off_t offset, int whence) {

}


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

