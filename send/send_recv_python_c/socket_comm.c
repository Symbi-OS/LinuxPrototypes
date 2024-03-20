#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_MSG_SIZE 1024

void child_process(int sock, int num_messages, int msg_size) {
    char message[MAX_MSG_SIZE];
    char msg[MAX_MSG_SIZE];

    memset(message, 'a', msg_size);

    for (int i = 0; i < num_messages; i++) {
        // Send a reply to parent
        send(sock, message, msg_size, 0);

        // Receive message from parent
        recv(sock, msg, msg_size, 0);
        // printf("Child received: %.10s...\n", msg);
    }

    close(sock);
}

void parent_process(int sock, int num_messages, int msg_size) {
    char message[MAX_MSG_SIZE];
    char reply[MAX_MSG_SIZE];

    memset(message, 'a', msg_size);

    for (int i = 0; i < num_messages; i++) {
        // Send a message to child
        send(sock, message, msg_size, 0);

        // Receive reply from child
        recv(sock, reply, msg_size, 0);
        // printf("Parent received: %.10s...\n", reply);
    }

    close(sock);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <num_messages> <msg_size>\n", argv[0]);
        exit(1);
    }

    int num_messages = atoi(argv[1]);
    int msg_size = atoi(argv[2]);

    if (msg_size > MAX_MSG_SIZE) {
        fprintf(stderr, "Message size exceeds the maximum allowed size.\n");
        exit(1);
    }

    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
        perror("socketpair");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        close(sv[0]);
        child_process(sv[1], num_messages, msg_size);
    } else {
        // Parent process
        close(sv[1]);
        parent_process(sv[0], num_messages, msg_size);
        wait(NULL);
    }

    return 0;
}


// mallocs on the hot path

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <sys/socket.h>
// #include <unistd.h>

// void child_process(int sock, int num_messages, int msg_size) {
//     char* message = (char*)malloc(msg_size);
//     memset(message, 'a', msg_size);

//     for (int i = 0; i < num_messages; i++) {
//         // Send a reply to parent
//         send(sock, message, msg_size, 0);

//         // Receive message from parent
//         char* msg = (char*)malloc(msg_size);
//         recv(sock, msg, msg_size, 0);
//         // printf("Child received: %.10s...\n", msg);
//         free(msg);
//     }

//     free(message);
//     close(sock);
// }

// void parent_process(int sock, int num_messages, int msg_size) {
//     char* message = (char*)malloc(msg_size);
//     memset(message, 'a', msg_size);

//     for (int i = 0; i < num_messages; i++) {
//         // Send a message to child
//         send(sock, message, msg_size, 0);

//         // Receive reply from child
//         char* reply = (char*)malloc(msg_size);
//         recv(sock, reply, msg_size, 0);
//         // printf("Parent received: %.10s...\n", reply);
//         free(reply);
//     }

//     free(message);
//     close(sock);
// }

// int main(int argc, char* argv[]) {
//     if (argc != 3) {
//         fprintf(stderr, "Usage: %s <num_messages> <msg_size>\n", argv[0]);
//         exit(1);
//     }

//     int num_messages = atoi(argv[1]);
//     int msg_size = atoi(argv[2]);

//     int sv[2];
//     if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
//         perror("socketpair");
//         exit(1);
//     }

//     pid_t pid = fork();
//     if (pid < 0) {
//         perror("fork");
//         exit(1);
//     }

//     if (pid == 0) {
//         // Child process
//         close(sv[0]);
//         child_process(sv[1], num_messages, msg_size);
//     } else {
//         // Parent process
//         close(sv[1]);
//         parent_process(sv[0], num_messages, msg_size);
//         wait(NULL);
//     }

//     return 0;
// }
