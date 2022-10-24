#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>


#define SERVER_KEY_PATHNAME "key-file"
#define PROJECT_ID 'M'


struct message_text {
    int qid;
    char buf [10];
};

struct message {
    long message_type;
    struct message_text message_text;
};

int main (int argc, char **argv)
{
    key_t server_queue_key;
    int server_qid, myqid;
    struct message my_message, return_message;
    clock_t start, end;
    double cpu_time_used;
    
    // create my client queue for receiving messages from server
    if ((myqid = msgget (IPC_PRIVATE, 0660)) == -1) {
        perror ("msgget: myqid");
        exit (1);
    }

    if ((server_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((server_qid = msgget (server_queue_key, 0)) == -1) {
        perror ("msgget: server_qid");
        exit (1);
    }

    my_message.message_type = 1;
    my_message.message_text.qid = myqid;

    // my message
    strcpy(my_message.message_text.buf, "aaaaaaaaaa");

    start = clock();
    for (int i=0; i<200000; i++) {
        // remove newline from string
        int length = strlen (my_message.message_text.buf);
        if (my_message.message_text.buf [length - 1] == '\n')
           my_message.message_text.buf [length - 1] = '\0';

        // send message to server
        if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("client: msgsnd");
            exit (1);
        }
	
        // read response from server
        if (msgrcv (myqid, &return_message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("client: msgrcv");
            exit (1);
        }

    }
    // remove message queue
    end = clock();
    cpu_time_used =  (double) (end-start);
    printf("Time used: %f\n", cpu_time_used/CLOCKS_PER_SEC);
    
    //Server Bye
    if (msgsnd (server_qid, &my_message, sizeof (struct message_text), 0) == -1) {
            perror ("client: msgsnd");
            exit (1);
    }
    
    if (msgctl (myqid, IPC_RMID, NULL) == -1) {
            perror ("client: msgctl");
            exit (1);
    }

    exit (0);
}
