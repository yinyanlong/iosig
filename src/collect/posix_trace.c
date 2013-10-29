#include <stdio.h>
#include <stdarg.h>
#include "iosig_trace.h"

typedef struct iosig_posix_file_t {
    int fh;              /* file handler */
    long int read_pos;   /* read position */
    long int write_pos;  /* write position */
    struct iosig_posix_file_t * next; /* we use link list for now */
} iosig_posix_file;

iosig_posix_file * bk_files_list;  /* head pointer of the book keeping 
                                      link list */

void IOSIG_posix_write_log (const char * operation, int fildes, long int position, 
        long int size, struct timeval * start, struct timeval * end, 
        const char * path) {
    struct timeval diffstart, diffend;
    timeval_diff(&diffstart, start, &bigbang);
    timeval_diff(&diffend, end, &bigbang);
    /* Format: OPEN, file_path, position, size, time1, time2 */

    if (path) {
        fprintf(posix_fp, "%-10s %3d %6ld %6ld %4ld.%06ld %4ld.%06ld %s\n", operation,
                fildes, position, size,
                (long) diffstart.tv_sec, (long) diffstart.tv_usec, 
                (long)diffend.tv_sec, (long) diffend.tv_usec, path);
    } else {
        fprintf(posix_fp, "%-10s %3d %6ld %6ld %4ld.%06ld %4ld.%06ld\n", operation,
                fildes, position, size,
                (long) diffstart.tv_sec, (long) diffstart.tv_usec, 
                (long)diffend.tv_sec, (long) diffend.tv_usec);
    }
}

/*
 * Book keeping for posix file operations.
 */
iosig_posix_file * IOSIG_posix_bk_open () {
    if (bk_files_list == NULL) {
        bk_files_list = malloc(sizeof(iosig_posix_file));
        return bk_files_list;
    } else {
        iosig_posix_file * tmp;
        tmp = bk_files_list;
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = malloc(sizeof(iosig_posix_file));
        tmp = tmp->next;
        tmp->next=NULL;

        return tmp;
    }
}

void IOSIG_posix_bk_close (int fildes) {
    iosig_posix_file * tmp = NULL;
    tmp = bk_files_list;

    if (bk_files_list == NULL) {
        return;
    } else {
        if (bk_files_list->fh == fildes) {
            tmp = bk_files_list;
            bk_files_list = bk_files_list->next;
            free(tmp);
            return;
        }

        tmp = bk_files_list;
        while (tmp->next != NULL && tmp->next->fh != fildes) {
            tmp = tmp->next;
        }
        if (tmp->next == NULL) {
            return;
        } else {
            iosig_posix_file * tmp2 = tmp->next;
            tmp->next = tmp2->next;
            free(tmp2);
            return;
        }
    }
}


/******** declare the real calls ********/
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

    iosig_posix_file * iosig_f = IOSIG_posix_bk_open(); 
    iosig_f->fh = ret_val;
    iosig_f->read_pos = 0;
    iosig_f->write_pos = 0; /* TODO */

    /* Format: OPEN, file_path, position, size, time1, time2 */
    IOSIG_posix_write_log ("OPEN", iosig_f->fh, 0, 0, &start, &end, path);
    return ret_val;
}

int __wrap_close(int fildes) {
    int ret_val;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    ret_val = __real_close(fildes);
    gettimeofday(&end, NULL);
    IOSIG_posix_bk_close (fildes);
    IOSIG_posix_write_log ("CLOSE", fildes, 0, 0, &start, &end, NULL);
    return ret_val;
}

ssize_t __wrap_read(int fildes, void *buf, size_t nbyte) {
    ssize_t ret_val;
    ret_val = __real_read(fildes, buf, nbyte);
    return ret_val;
}

ssize_t __wrap_write(int fildes, const void *buf, size_t nbyte) {
    ssize_t ret_val;
    ret_val = __real_write(fildes, buf, nbyte);
    return ret_val;
}

off_t __wrap_lseek(int fildes, off_t offset, int whence) {
    off_t ret_val;
    ret_val = __real_lseek(fildes, offset, whence);
    return ret_val;
}


FILE* __wrap_fopen (const char *path, const char *mode) {
    FILE * ret;
    ret =  __real_fopen(path, mode);
    return ret;
}

int __wrap_fclose (FILE* stream) {
    int ret_val = __real_fclose(stream);
    return ret_val;
}

size_t __wrap_fread(void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    size_t ret_val = __real_fread(ptr, size, nitems, stream);
    return ret_val;
}

size_t __wrap_fwrite(const void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    size_t ret_val = __real_fwrite(ptr, size, nitems, stream);
    return ret_val;
}

int __wrap_fseek(FILE *stream, long offset, int whence) {
    int ret_val = __real_fseek(stream, offset, whence);
    return ret_val;
}

