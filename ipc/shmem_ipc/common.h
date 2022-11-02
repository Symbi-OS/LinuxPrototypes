#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define UNUSED(param) (void)param;

//Job buffer definitions
#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2
#define JOB_BUFFER_IN_USE 3 // Indicating the client registered this job buffer
#define SERVER_KILL_SIGNAL 10
#define CMD_WRITE 1
#define CMD_GETPPID 2

//server configs
#define BACKING_FILE "sym_server_shm"
#define BACKING_FILE_SIZE 512
#define MAX_JOB_BUFFERS 2
#define MAX_BUF_SIZE 100

typedef struct __attribute__((__packed__)) job_buffer {
    int cmd;           // Job command requested by the client
    int response;      // Response from the server
    char buf[MAX_BUF_SIZE]; // hold arguments
    volatile int status;    // Flag indicating which stage the job is at
} job_buffer_t;

typedef struct workspace {
    job_buffer_t job_buffers[MAX_JOB_BUFFERS];
} workspace_t;

#define START_CLOCK() clock_t _start_time = clock();
#define STOP_CLOCK() clock_t _end_time = clock();
#define GET_DURATION() ((double)(_end_time - _start_time)) / CLOCKS_PER_SEC;

typedef int(*ksys_write_t)(unsigned int fd, const char* buf, size_t count);
typedef int (*getppid_t)(void);

void* server_init();
void* connect_server();
job_buffer_t *get_job_buffer(workspace_t *workspace);
void* job_buffer_thread(void *request_job_buffer);
int server_write(job_buffer_t * job_buffer, char * floc, char * buf, size_t len);
int server_getppid(job_buffer_t * job_buffer);

