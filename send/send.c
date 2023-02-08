#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

// include for wait
#include <sys/wait.h>

// #include <string.h>
// #include <sys/mman.h>
// #include <linux/mman.h>

#define ADDR_HINT 0x300000000000
#define sock "./my_sock"
#define DEBUG 0
// #define fprintf //fprintf

void send_bench(unsigned msg_size,  unsigned num_runs ) {
    fprintf(stderr, "starting send_bench\n");

    // need to set affinity of parent and child to different cpus
    int retval, forkId, status, l;
    int fds1[2], fds2[2];
    char w = 'b', r;
    char *buf;
    struct sockaddr_un server_addr;

    memset(&server_addr, 0, sizeof(struct sockaddr_un));
    server_addr.sun_family = AF_UNIX;

    strncpy(server_addr.sun_path, sock, sizeof(server_addr.sun_path) - 1);

    // create a buffer (this needs to be mmap)
    buf = (char *)malloc(sizeof(char) * msg_size);
    // write character 'a' in the buffer
    for (int i = 0; i < msg_size; i++) {
        buf[i] = 'a';
    }

    fprintf(stderr, "about to num_runs\n");
    for (l = 0; l < num_runs; l++) {
        retval = pipe(fds1);
        if (retval != 0)
            fprintf(stderr, "[error] failed to open pipe1.\n");
        retval = pipe(fds2);
        if (retval != 0)
            fprintf(stderr, "[error] failed to open pipe1.\n");

        fprintf(stderr, "about to fork\n");
        forkId = fork();
        fprintf(stderr, "back from fork\n");

        if (forkId < 0) {
            fprintf(stderr, "[error] fork failed.\n");
            return;
        }
        if (forkId == 0) {
            close(fds1[0]); // close the read end of pipe 1
            close(fds2[1]); // close the write end of pipe 2
            printf("in child, closed pipes\n");

            int fd_server =
                socket(AF_UNIX, SOCK_STREAM, 0); // create a socket for server
            if (fd_server < 0)
                fprintf(stderr, "[error] failed to open server socket.\n");

            // bind the socket to a filename so communication can occur
            printf("created server socket, about to bind\n");
            retval = bind(fd_server, (struct sockaddr *)&server_addr,
                          sizeof(struct sockaddr_un));
            if (retval == -1)
                fprintf(stderr, "[error] failed to bind.\n");

            // Listen on the socket
            retval = listen(fd_server, 10);
            if (retval == -1)
                fprintf(stderr, "[error] failed to listen.\n");
            if (DEBUG)
                fprintf(stderr, "Waiting for connection\n");

            // Write to pipe 1 to let parent know we are listening
            write(fds1[1], &w, 1);

            // wait for connection
            int fd_connect =
                accept(fd_server, (struct sockaddr *)0, (socklen_t *)0);
            fprintf(stderr, "conn accepted\n");
            if (DEBUG)
                fprintf(stderr, "Connection accepted.\n");

            // Read update from parent on pipe 2
            read(fds2[0], &r, 1);

            // remove sockets and close file descriptors and pipes
            remove(sock);
            close(fd_server);
            close(fd_connect);
            close(fds1[1]);
            close(fds2[0]);

            fprintf(stderr, "about to exit\n");
            // job done, time to exit
            exit(0);

        } else {
            fprintf(stderr, "in parent\n");
            close(fds1[1]); // close the write end of pipe 1
            close(fds2[0]); // close the read end of pipe 2

            // Wait for update from child
            read(fds1[0], &r, 1);
            fprintf(stderr, "got child update\n");

            // create socket
            int fd_client = socket(AF_UNIX, SOCK_STREAM, 0);
            if (fd_client < 0)
                fprintf(stderr, "[error] failed to open client socket.\n");

            // connect to child address
            retval = connect(fd_client, (struct sockaddr *)&server_addr,
                             sizeof(struct sockaddr_un));
            if (retval == -1)
                fprintf(stderr, "[error] failed to connect.\n");
            fprintf(stderr, "conn done\n");
            // send the buffer over to child (for warm-up)
            fprintf(stderr, "about to send warm up\n");
            retval = send(fd_client, buf, msg_size, MSG_DONTWAIT);
            if (retval == -1) {
                fprintf(stderr, "[error %d] failed to send. %s\n", errno,
                       strerror(errno));
            }

            fprintf(stderr, "about to send\n");
            retval = syscall(SYS_sendto, fd_client, buf, msg_size, MSG_DONTWAIT,
                             NULL, 0);

            if (retval == -1) {
                fprintf(stderr, "[error %d] failed to send. %s\n", errno,
                       strerror(errno));
            }
            fprintf(stderr, "done send\n");

            // update child so clean-up can happen
            write(fds2[1], &w, 1);

            // do cleanup your-self
            close(fd_client);
            close(fds1[0]);
            close(fds2[1]);

            wait(&status);

        }
    }
    free(buf);
    fprintf(stderr, "done, returning\n");
}

int main(int argc, char *argv[]) {
    unsigned msg_size, num_msgs;
    if (argc != 3) {
        fprintf(stderr, "Usage: ./send_bench <msg_size> <num_of_msgs>\n");
        exit(1);
    }
    msg_size = atoi(argv[1]);
    num_msgs = atoi(argv[2]);

    send_bench(msg_size, num_msgs);

    return 0;
}