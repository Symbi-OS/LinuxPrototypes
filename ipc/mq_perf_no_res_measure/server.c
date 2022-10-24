#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "LINF/sym_all.h"
#include <time.h>

#define STRESS_TEST_ITERATIONS 200000

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

#ifdef ELEVATED_MODE
typedef int(*ksys_write_t)(unsigned int fd, const char *buf, size_t count);

static ksys_write_t my_ksys_write = NULL;
#endif


int main ()
{
    key_t msg_queue_key;
    int qid;
    struct message message;
    char *filename = "tmp.txt";
    
    
    #ifdef ELEVATED_MODE
	// Init symbiote library and kallsymlib
	sym_lib_init();
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	sym_elevate();

    #endif

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

    clock_t start, end;
    double cpu_time_used = 0;

    int i = 0;
    while (1) {
        // read an incoming message
        if (msgrcv (qid, &message, sizeof (struct message_text), 0, 0) == -1) {
            perror ("msgrcv");
            exit (1);
        }
	
	if (i == 0){
            start = clock();
	}

        // process message
        int length = strlen (message.message_text.buf);
        
	#ifdef ELEVATED_MODE
			my_ksys_write(fd, message.message_text.buf, length);
	#else
			write(fd, message.message_text.buf, length);
	#endif
	
	i++;
        if (i == STRESS_TEST_ITERATIONS){
            end = clock();
	    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  	    printf("server time used: %f\n", cpu_time_used);
                exit(0);
        }


    }
}

