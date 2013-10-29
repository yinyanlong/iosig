#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
 
int stream_io(void) {
    char buffer[5] = {'a'};  /* initialized to zeroes */
    FILE *fp = fopen("myfile", "w+");

    if (fp == NULL) {
        perror("Failed to open file \"myfile\"");
        return EXIT_FAILURE;
    }

    fwrite(buffer, 1, 5, fp);
                         
    fclose(fp);

    return EXIT_SUCCESS;
}

int posix_io(void) {
    char filename[64];
    char * buffer = "aaaaaa";
    int file_handlers[5];
    int i = 0;

    /* open */
    for (i = 0; i < 5; i++) {
        sprintf(filename, "./hehe_%d", i);
        file_handlers[i] = open(filename, O_WRONLY | O_CREAT);
    }

    /* write */
    for (i = 0; i < 5; i++) {
        write(file_handlers[i], buffer, 5);
    }

    /* close */
    for (i = 4; i >=0; i--) {
        close(file_handlers[i]);
    }
    
    return EXIT_SUCCESS;
}

int main(void) {
    //stream_io();
    posix_io();
    return 0;
}
