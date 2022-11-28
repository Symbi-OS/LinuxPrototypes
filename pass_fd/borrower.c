// A program that borrows a file descriptor from another process
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/syscall.h>

#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char *argv[]) {
    // Give owner a sec to set up
    sleep(1);


    // Read pid.txt and fd.txt for the pid and fd
    FILE *pid_file = fopen("pid.txt", "r");
    FILE *fd_file = fopen("fd.txt", "r");
    int pid, fd;
    fscanf(pid_file, "%d", &pid);
    fscanf(fd_file, "%d", &fd);
    fclose(pid_file);
    fclose(fd_file);

    printf("found pid %d and fd %d\n", pid, fd);

    // struct timespec ts;
    // clock_gettime(CLOCK_REALTIME, &ts);
    // printf("borrower trying to get fd: %ld\n", ts.tv_sec);

    // Borrow the file descriptor
    printf("about to try to borrow fd\n");
    // This is different from a pid.
    int pidfd = syscall(SYS_pidfd_open, pid, 0);


    int borrowed_fd = syscall(SYS_pidfd_getfd, pidfd, fd, 0);
    printf("done trying to borrow fd\n");

    // check error case
    if (borrowed_fd == -1) {
        perror("pidfd_getfd");
        //  print errno
        printf("errno: %d\n", errno);
        //  print strerror
        printf("strerror: %s\n", strerror(errno));
        return 1;
    }

    printf("got borrowed fd %d\n", borrowed_fd);

    // Write to borrowed_fd
    if (write(borrowed_fd, "World!", 7) == -1) {
        perror("write");
        return 1;
    }
    printf("ending borrower\n");
    return 0;
}
