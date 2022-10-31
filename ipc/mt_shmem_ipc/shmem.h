#ifndef SHMEM_H_  /* Include guard */
#define SHMEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// server config
#define BACKING_FILE "process_shared_memory"
#define BACKING_FILE_SIZE 512
#define NUM_JOB_BUFFERS 2
#define MAX_BUF_SIZE 100
#define FILENAME_SIZE 10

// Request status
#define REQUEST_CREATED  1
#define REQUEST_SENT  2
#define REQUEST_RECEIVED 3
#define REQUEST_PROCESSING 4
#define REQUEST_COMPLETED 5
#define REQUEST_DONE 6

//JOB_BUFFER status
#define EMPTY_JOB_BUFFER 0
#define JOB_BUFFER_IN_USE 10
#define KILL_SIGNAL 11

// server commands
#define CMD_WRITE 1

typedef struct job_buffer {
    char filename[FILENAME_SIZE];            // File descriptor
    int cmd;           // Job command requested by the client
    size_t buf_len;
    char buf[MAX_BUF_SIZE];
    volatile int status;        // Flag indicating which stage the job is at
} job_buffer_t;

typedef struct workspace {
    job_buffer_t job_buffers[NUM_JOB_BUFFERS];
} workspace_t;

void* server_init();
void* connect_server();
job_buffer_t *get_job_buffer(workspace_t *workspace);
void* job_buffer_thread(void *request_job_buffer);

#endif
