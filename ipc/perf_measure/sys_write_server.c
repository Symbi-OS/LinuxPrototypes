#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>


#define SERVER_KEY_PATHNAME "key-file"
#define PROJECT_ID 'M'
#define QUEUE_PERMISSIONS 0660

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
    key_t msg_queue_key;
    int qid;
    struct message message;
    char *filename = "tmp.txt";

    printf("server is up\n");
    if ((msg_queue_key = ftok (SERVER_KEY_PATHNAME, PROJECT_ID)) == -1) {
        perror ("ftok");
        exit (1);
    }

    if ((qid = msgget (msg_queue_key, IPC_CREAT | QUEUE_PERMISSIONS)) == -1) {
        perror ("msgget");
        exit (1);
    }

    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }

    while (1) {
        // read an incoming message
        if (msgrcv (qid, &message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }

       
        // process message
        int length = strlen (message.message_text.buf);
        
	write(fd, message.message_text.buf, length);

	/*
	strcpy(message.message_text.buf, "request completed");
        

        int client_qid = message.message_text.qid;
        message.message_text.qid = qid;

        // send reply message to client
        if (msgsnd (client_qid, &message, sizeof (struct message_text), 0) == -1) {  
            perror ("msgget");
            exit (1);
        }

        printf ("Responded to the app.\n");
	*/
    }
}
