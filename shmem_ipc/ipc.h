#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// Job Status Definitions
#define JOB_NO_REQUEST 0
#define JOB_BUFFER_IN_USE 1
#define JOB_REQUESTED  2
#define JOB_PICKEDUP 3
#define JOB_COMPLETED  4


// Server Config
#define MAX_JOB_BUFFERS 7

// Commands
#define CMD_KILL_SERVER  -1
#define CMD_DISCONNECT  0
#define CMD_WRITE 1
#define CMD_OPEN  2
#define CMD_CLOSE 3

typedef struct JobRequestBuffer {
    int pid;              // Client's PID
    int cmd;              // Job command requested by the client
    int arg1;             // First integer argument
    int arg2;             // Second integer argument
    int response;         // Response from the server
	char buffer[128];	  // Command buffer
	int buffer_len;		  // Commabd buffer length
    volatile int status;  // Flag indicating which stage the job is at
} JobRequestBuffer_t;

typedef struct workspace {
    JobRequestBuffer_t job_buffers[MAX_JOB_BUFFERS];
} workspace_t;

#define SHMEM_REGION_SIZE 0x1000

void* ipc_connect_client();
void  ipc_close();
workspace_t*  ipc_connect_server();
JobRequestBuffer_t* ipc_get_job_buffer();

void submit_job_request(JobRequestBuffer_t* jrb);
void wait_for_job_completion(JobRequestBuffer_t* jrb);

void mark_job_completed(JobRequestBuffer_t* jrb);
void wait_for_job_request(JobRequestBuffer_t* jrb);
void disconnect_job_buffer(JobRequestBuffer_t* jrb);
void print_job_buffer(JobRequestBuffer_t* jrb)