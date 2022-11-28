// A program that lends a file descriptor
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// A function that takes a file name and char array and writes it to the file
void write_to_file(char *file_name, char *string) {
    int fd = open(file_name, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(1);
    }
    write(fd, string, strlen(string));
    close(fd);
}

void write_int_to_file(char *file_name, int num) {
    // Num is an int, turn it into a string, send it to write_to_file
    char *num_as_str = malloc(128);
    sprintf(num_as_str, "%d", num);

    write_to_file(file_name, num_as_str);
    free(num_as_str);
}

int main(int argc, char *argv[]) {
    // Open a new file, test.txt
    printf("starting sym_owner\n");

    int fd = open("test.txt", O_WRONLY | O_CREAT, 0644);
    // check error case
    if (fd == -1) {
        perror("open");
        return 1;
    }
    printf("owners pid %d\n", getpid());
    printf("owners fd is %d\n", fd);

    printf("fd is open\n");

    // Write "Hello, " to the file
    if (write(fd, "Hello, ", 7) == -1) {
        perror("write");
        return 1;
    }

    int pid = getpid();
    write_int_to_file("pid.txt", pid);
    write_int_to_file("fd.txt", fd);

    // sleep for 2
    sleep(2);

    // Write " Bye!" to the file
    if (write(fd, " Bye!", 6) == -1) {
        perror("write");
        return 1;
    }
    // Close the file
    printf("about to close fd\n");
    close(fd);
    printf("closed fd\n");

    printf("ending sym_owner\n");
    return 0;
}