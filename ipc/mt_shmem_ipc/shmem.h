#ifndef SHMEM_H_  /* Include guard */
#define SHMEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define DEBUG 0

// server config
#define BACKING_FILE "process_shared_memory"
#define BACKING_FILE_SIZE 512
#define NUM_SLOTS 2
#define MAX_BUF_SIZE 100
#define FILENAME_SIZE 10

// Request status
#define REQUEST_CREATED  0x1
#define REQUEST_SENT  0x2
#define REQUEST_RECEIVED 0x3
#define REQUEST_PROCESSING 0x4
#define REQUEST_COMPLETED 0x5
#define REQUEST_DONE 0x6

//slot status
#define EMPTY_SLOT 0x0
#define SLOT_IN_USE 0xa

// server commands
#define CMD_WRITE 0x1

typedef struct slot {
    char filename[FILENAME_SIZE];            // File descriptor
    char cmd;           // Job command requested by the client
    size_t buf_len;
    char buf[MAX_BUF_SIZE];
    char status;        // Flag indicating which stage the job is at
} slot_t;

typedef struct workspace {
    slot_t slot[NUM_SLOTS];
} workspace_t;

void* server_init();
void* connect_server();
slot_t *get_slot(workspace_t *workspace);
void* slot_thread(void *request_slot);

#endif
