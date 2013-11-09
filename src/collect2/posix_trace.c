#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#include "iosig_trace.h"

#define IOSIG_ASSERT_TEST 1

typedef struct iosig_posix_file_t {
    int fh;         /* file handler */
    char lseeked;   /* `lseeked' being 1 means `lseek' has been called.
                       The consequence `write' call will reset the offset
                       to `size of the file'. It's set to `1' when lseek
                       gets called, and reset to `0' when `write' is 
                       called */
    int oflag;
    off64_t offset;
    struct iosig_posix_file_t * next; /* we use link list for now */
} iosig_posix_file;

iosig_posix_file * bk_files_list;  /* head pointer of the book keeping 
                                      link list */
                                   /* TODO: make the access to bk_files_list
                                    * thread safe by adding locks.  */

/*
 * Format: OP, FD, POS, SIZE, T1, T2, PATH
 */
void IOSIG_posix_write_log (const char * operation, int fildes, off64_t position, 
        size_t size, struct timeval * start, struct timeval * end, 
        const char * path) {
    struct timeval diffstart, diffend;
    timeval_diff(&diffstart, start, &bigbang);
    timeval_diff(&diffend, end, &bigbang);
    /* Format: OPEN, file_path, position, size, time1, time2 */

    if (path) {
        sprintf(posix_logtext, "%-10s %3d %6ld %6ld %4ld.%06ld %4ld.%06ld %s\n", 
                operation, fildes, position, size,
                (long) diffstart.tv_sec, (long) diffstart.tv_usec, 
                (long) diffend.tv_sec, (long) diffend.tv_usec, path);
        __real_fwrite(posix_logtext, strlen(posix_logtext), 1, posix_fp);
    } else {
        sprintf(posix_logtext, "%-10s %3d %6ld %6ld %4ld.%06ld %4ld.%06ld\n", 
                operation, fildes, position, size,
                (long) diffstart.tv_sec, (long) diffstart.tv_usec, 
                (long)diffend.tv_sec, (long) diffend.tv_usec);
        __real_fwrite(posix_logtext, strlen(posix_logtext), 1, posix_fp);
    }
}

/*
 * Book keeping for posix file operations.
 */

/* 
 * Return the newly created entry in the book keeping list.
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

/* 
 * Remove the entry identified with `fildes'.
 */
void IOSIG_posix_bk_close (int fildes) {
    iosig_posix_file * tmp = bk_files_list;

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

/*
 * Reture the book keeping entry of the given `fildes'.
 */
iosig_posix_file * IOSIG_posix_get_file_by_fd (int fildes) {
    if (bk_files_list == NULL) {
        return NULL;
    } 
    iosig_posix_file * tmp = bk_files_list;
    while (tmp!=NULL && tmp->fh != fildes) {
        tmp = tmp->next;
    }
    if (tmp == NULL) {
        return NULL;
    } else {
        return tmp;
    }
}

/*
 * Update the book keeping entry of the given `fildes'.
 * The second parameter is the read/write size of this I/O operation.
 * The return value is the beginning offset of this operation.
 */
off_t IOSIG_posix_bk_read (int fildes, size_t nbyte) {
    iosig_posix_file * tmp = IOSIG_posix_get_file_by_fd(fildes);
    if (tmp == NULL) {
        return -1;
    } 
    off_t ret_val = tmp->offset;
    tmp->offset += nbyte;

#if IOSIG_ASSERT_TEST
    off_t new_offset = lseek(fildes, 0, SEEK_CUR);
    assert (new_offset == tmp->offset);
#endif

    return ret_val;
}

off_t IOSIG_posix_bk_write (int fildes, size_t nbyte) {
    iosig_posix_file * tmp = IOSIG_posix_get_file_by_fd(fildes);
    if (tmp == NULL) {
        return -1;
    } 
    if ( (tmp->oflag & O_APPEND) > 0 && tmp->lseeked == 1) {
        /* Must check whether O_APPEND is in `oflag'. If it's on, then
         * the offset goes to end, and then write the data 
         */
        tmp->lseeked = 0;
        off_t new_offset = lseek(fildes, 0, SEEK_CUR);
        tmp->offset = new_offset;
        return (new_offset - nbyte);
    }
    off_t ret_val = tmp->offset;
    tmp->offset += nbyte;

#if IOSIG_ASSERT_TEST
    off_t new_offset = lseek(fildes, 0, SEEK_CUR);
    assert (new_offset == tmp->offset);
#endif

    return ret_val;
}

/* Return value is the old offset before seeking. */
off64_t IOSIG_posix_bk_lseek (int fildes, off64_t new_offset) {
    iosig_posix_file * tmp = IOSIG_posix_get_file_by_fd(fildes);
    if (tmp == NULL) {
        return -1;
    } 
    off64_t ret_val = tmp->offset;
    tmp->offset = new_offset;
    tmp->lseeked = 1;
    return ret_val;
}

/* utility functions */

/******** the wrap calls ********/
int __wrap_open64(const char *path, int oflag, ... ) {
    int ret_val;
    struct timeval start, end;

    /* check whether there is the 3rd arg */
    if (oflag & O_CREAT) {
        va_list mode_arg;
        va_start(mode_arg, oflag);
        int mode = va_arg(mode_arg, int);
        va_end(mode_arg);

        gettimeofday(&start, NULL);
        ret_val = __real_open64(path, oflag, mode);
        gettimeofday(&end, NULL);
    } else {
        gettimeofday(&start, NULL);
        ret_val = __real_open64(path, oflag);
        gettimeofday(&end, NULL);
    }

    if (ret_val == -1) { /* Upon error.  */
        return ret_val;
    }
    iosig_posix_file * iosig_f = IOSIG_posix_bk_open(); 
    iosig_f->fh = ret_val;
    iosig_f->oflag = oflag;
    iosig_f->offset = 0;

    /* Check on flag `O_APPEND'.  */
    struct stat st;
    if ( (oflag&O_APPEND) > 0 && stat(path, &st)==0 ) {
        iosig_f->offset = st.st_size;
    } else {
        iosig_f->offset = 0;
    }

    /* Format: OPEN, file_path, position, size, time1, time2, path */
    IOSIG_posix_write_log ("OPEN", iosig_f->fh, 0, 0, &start, &end, path);
    return ret_val;
}
int __wrap_open(const char *path, int oflag, ... ) {
    if (oflag & O_CREAT) {
        va_list mode_arg;
        va_start(mode_arg, oflag);
        int mode = va_arg(mode_arg, int);
        va_end(mode_arg);

        return __wrap_open64(path, oflag, mode);
    } else {
        return __wrap_open64(path, oflag);
    }
}
#define __wrap_open __wrap_open64

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
    struct timeval start, end;

    gettimeofday(&start, NULL);
    ret_val = __real_read(fildes, buf, nbyte);
    gettimeofday(&end, NULL);
    if (ret_val >= 0) {         /* ret_val is actual read bytes */
        off_t offset = IOSIG_posix_bk_read (fildes, ret_val);
        IOSIG_posix_write_log ("READ", fildes, offset, ret_val, &start, &end, NULL);
    }
    return ret_val;
}

ssize_t __wrap_write(int fildes, const void *buf, size_t nbyte) {
    ssize_t ret_val;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    ret_val = __real_write(fildes, buf, nbyte);
    gettimeofday(&end, NULL);

    if (ret_val >= 0) {         /* ret_val is actual write bytes */
        off_t offset = IOSIG_posix_bk_write (fildes, ret_val);
        IOSIG_posix_write_log ("WRITE", fildes, offset, ret_val, &start, &end, NULL);
    }

    return ret_val;
}

off64_t __wrap_lseek64(int fildes, off64_t offset, int whence) {
    off64_t new_offset; /* the new offset */
    struct timeval start, end;

    gettimeofday(&start, NULL);
    new_offset = __real_lseek(fildes, offset, whence);
    gettimeofday(&end, NULL);

    if (offset != 0 && new_offset != -1) {
        off64_t old_offset = IOSIG_posix_bk_lseek (fildes, new_offset);
        IOSIG_posix_write_log ("LSEEK", fildes, old_offset, new_offset, &start, &end, NULL);
    }
    return new_offset;
}
off_t __wrap_lseek(int fildes, off_t offset, int whence) {
    return (off_t) __wrap_lseek64(fildes, (off64_t)offset, whence);

    /*
    off_t new_offset;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    new_offset = __real_lseek(fildes, offset, whence);
    gettimeofday(&end, NULL);

    if (offset != 0 && new_offset != -1) {
        off_t old_offset = IOSIG_posix_bk_lseek (fildes, new_offset);
        IOSIG_posix_write_log ("LSEEK", fildes, old_offset, new_offset, &start, &end, NULL);
    }

    return new_offset;
    */
}


FILE* __wrap_fopen64 (const char *path, const char *mode) {
    FILE * ret_val;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    ret_val =  __real_fopen64(path, mode);
    gettimeofday(&end, NULL);

    IOSIG_posix_write_log ("FOPEN", ret_val->_fileno, 0, 0, &start, &end, path); 
    return ret_val;
}
FILE* __wrap_fopen (const char *path, const char *mode) {
    return __wrap_fopen64(path, mode);
}

int __wrap_fclose (FILE* stream) {
    int ret_val;
    int file_no = stream->_fileno;
    struct timeval start, end;

    gettimeofday(&start, NULL);
    ret_val = __real_fclose(stream);
    gettimeofday(&end, NULL);

    IOSIG_posix_write_log ("FCLOSE", file_no, 0, 0, &start, &end, NULL); 
    return ret_val;
}

size_t __wrap_fread(void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    if (stream->_fileno < 3) {
        /* fileno 0, 1, and 2 represent stdin, stdout, stderr  */
        return __real_fread(ptr, size, nitems, stream);
    }

    struct timeval start, end;
    fpos64_t old_offset;
    fgetpos64(stream, &old_offset); /* TODO: check ret_val of fgetpos  */

    gettimeofday(&start, NULL);
    size_t ret_val = __real_fread(ptr, size, nitems, stream);
    gettimeofday(&end, NULL);

    if (ret_val > 0) {
        IOSIG_posix_write_log ("FREAD", stream->_fileno, 
                (off64_t) old_offset.__pos, 
                ret_val, &start, &end, NULL); 
    }
    return ret_val;
}

size_t __wrap_fwrite(const void * ptr, size_t size, size_t nitems, 
        FILE * stream) {
    if (stream->_fileno < 3) {
        /* fileno 0, 1, and 2 represent stdin, stdout, stderr  */
        return __real_fwrite(ptr, size, nitems, stream);
    }
    
    struct timeval start, end;
    /* stream->_markers is NULL  */
    fpos64_t old_offset;
    fgetpos64(stream, &old_offset); /* TODO: check ret_val of fgetpos  */

    gettimeofday(&start, NULL);
    size_t ret_val = __real_fwrite(ptr, size, nitems, stream);
    gettimeofday(&end, NULL);

    if (ret_val > 0) { 
        IOSIG_posix_write_log ("FWRITE", stream->_fileno, 
                old_offset.__pos, 
                ret_val, &start, &end, NULL); 
    }
    return ret_val;
}

int __wrap_fseek(FILE *stream, long offset, int whence) {
    struct timeval start, end;
    fpos64_t old_offset;
    fgetpos64(stream, &old_offset); /* TODO: check ret_val of fgetpos  */

    gettimeofday(&start, NULL);
    int ret_val = __real_fseek(stream, offset, whence);
    gettimeofday(&end, NULL);

    if (ret_val > 0) {
        IOSIG_posix_write_log ("FSEEK", stream->_fileno, 
                old_offset.__pos, 
                ret_val, &start, &end, NULL); 
    }
    return ret_val;
}

