#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WRITE_BUF "BlackMagic\n"
#define WRITE_LOC "tmp.txt"

void write_task(int iteration, slot_t *work_slot){

    FILE * fp;
    int fd;

    fp = fopen(WRITE_LOC, "a");
    fd = fileno(fp);

    //printf("%p\n", work_slot);
    
    for (int i = 0; i < iteration; i++){
        
        if (work_slot == NULL){
            write(fd, WRITE_BUF, strlen(WRITE_BUF));
        }else{
            //initialze the request
            work_slot->status = REQUEST_CREATED;
            work_slot->cmd = CMD_WRITE;
            work_slot->buf_len = strlen(WRITE_BUF);
            strcpy(work_slot->buf, WRITE_BUF);
            strcpy(work_slot->filename, WRITE_LOC);
            //send request
            work_slot->status = REQUEST_SENT;
            if (DEBUG) printf("request %ld byte to write in %s\n", work_slot->buf_len, work_slot->filename);
            // Wait for the job to be completed
            while (work_slot->status != REQUEST_COMPLETED ) {
                continue;
            }

            if (DEBUG) printf("recevied response from server\n");
		}
    }

    //complete work
    if (work_slot != NULL){
        work_slot->status = EMPTY_SLOT;
    }

    return;
}


int main(int argc, char** argv){

    int iterations;
    int if_connect;

    if (argc == 3){
        iterations = atoi(argv[1]);
        if_connect = atoi(argv[2]);
    }else{
        iterations = 50;
        if_connect = 0;
    }

    if (!if_connect){
        write_task(iterations, NULL);
        return 0;
    }

    workspace_t *workspace = connect_server();

    if (workspace == (void *)-1){
        printf("Fail to connect to server...\n");
        return -1;
    }

    slot_t * work_slot = get_slot(workspace);
    if (work_slot == (void *)-1) {
        printf("Fail to request working slot...\n");
        return 1;
    }

    if (DEBUG) printf("connected to slot at %p\n", work_slot);

    
    write_task(iterations, work_slot);
	

    //printf("Server c at: %p\n", workspace);

    return 0;
}