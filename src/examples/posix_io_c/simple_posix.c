#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
 
int stream_io(void) {
    fpos_t fpos;
    int i;
    char buffer[5] = {'b'};  /* initialized to zeroes */
    printf("-----------------------1-------------------\n");
    FILE *fp = fopen("./myfile", "w+");
    printf("-----------------------2-------------------\n");

    if (fp == NULL) {
        perror("Failed to open file \"myfile\"");
        return EXIT_FAILURE;
    }

    printf("-----------------------3-------------------\n");
    for (i = 0; i < 5; i++) {
        fwrite(buffer, 1, 5, fp);
    }
    printf("-----------------------4-------------------\n");
                         
    fprintf(fp, "bbbbb");
    fclose(fp);

    return EXIT_SUCCESS;
}

int posix_io(void) {
    char filename[64];
    char * buffer = "eeeeee";
    char read_buffer[5];
    int file_handlers[5];
    int i = 0;
    int j = 0;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    /* open */
    for (i = 0; i < 5; i++) {
        sprintf(filename, "./hehe_%d", i);
        file_handlers[i] = open(filename, O_RDWR | O_APPEND | O_CREAT, mode);
    }

    /* write */
    int offset;
    for (i = 0; i < 5; i++) {
        offset = lseek(file_handlers[i], 200, SEEK_SET);
        printf("xxxxxxxxxxxxx      new offset: %d\n", offset);
    }

    for (i = 0; i < 5; i++) {
        for (j = 0; j<5; j++) {
            write(file_handlers[i], buffer, 5);
        }
    }
    
    /*
    for (i = 4; i >=0; i--) {
        close(file_handlers[i]);
    }

    for (i = 0; i < 5; i++) {
        sprintf(filename, "./hehe_%d", i);
        file_handlers[i] = open(filename, O_RDWR | O_CREAT | O_APPEND, mode);
    }
    */

    /*
    int offset;
    for (i = 0; i < 5; i++) {
        offset = lseek(file_handlers[i], -500, SEEK_CUR);
        printf("new offset: %d\n", offset);
    }
    */
    for (i = 0; i < 5; i++) {
        offset = lseek(file_handlers[i], 0, SEEK_CUR);
        printf("+++++++++++++++++current offset: %d\n", offset);
    }
    for (i = 0; i < 5; i++) {
        offset = lseek(file_handlers[i], -200, SEEK_CUR);
        printf("+++++++++++++++++new offset: %d\n", offset);
    }

    for (i = 0; i < 5; i++) {
        for (j = 0; j<5; j++) {
            read(file_handlers[i], read_buffer, 5);
        }
    }
    

    /* close */
    for (i = 4; i >=0; i--) {
        close(file_handlers[i]);
    }
    
    return EXIT_SUCCESS;
}

int main(void) {
    stream_io();
    //posix_io();
    return 0;
}

