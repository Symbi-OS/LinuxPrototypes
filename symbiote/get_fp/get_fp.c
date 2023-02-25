// A program that opens a file and gets the kernel's corresponding file pointer.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "LINF/sym_all.h"

typedef void *(*__fdget_t)(int);
void get_fp(int fd, void **fp){
    __fdget_t my__fdget = NULL;

    if (my__fdget == NULL) {
        my__fdget = (__fdget_t) sym_get_fn_address("__fdget");
        if (my__fdget == NULL) {
            fprintf(stderr, "failed to find __fdget() \n");
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "got __fdget() at %p \n", my__fdget);
    sym_elevate();
    *fp = my__fdget(fd);
    fprintf(stderr, "finished __fdget() \n");

}

int main(int argc, char *argv[])
{
    int fd;
    void *fp;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    get_fp(fd, &fp);

    fprintf(stderr, "File descriptor: %d \n", fd);
    fprintf(stderr, "File pointer: %p \n", fp);

    exit(EXIT_SUCCESS);
}