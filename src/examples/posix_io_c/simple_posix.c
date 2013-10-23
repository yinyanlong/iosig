#include <stdio.h>
#include <stdlib.h>
 
int main(void)
{
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

