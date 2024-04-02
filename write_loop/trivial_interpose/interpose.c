#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

ssize_t write(int fd, const void *buf, size_t count) {
    static ssize_t (*real_write)(int, const void *, size_t) = NULL;

    if (!real_write) {
        real_write = dlsym(RTLD_NEXT, "write");
        if (!real_write) {
            fprintf(stderr, "Error: failed to find the original write function\n");
            return -1;
        }
    }

    // printf("we called write\n");
    return real_write(fd, buf, count);
}