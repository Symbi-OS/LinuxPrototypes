#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2
#define BUF_IN_USE  3
#define MAX_CLIENT_PER_BUF 1

typedef struct JobRequestBuffer {
    int fd;               // File descriptor
    int cmd;              // Job command requested by the client
    int response;         // Response from the server
	char buffer[128];	  // Command buffer
	int buffer_len;		  // Commabd buffer length
    volatile int status[MAX_CLIENT_PER_BUF];  // Flag indicating which stage the job is at
} JobRequestBuffer_t;

#define SHMEM_REGION_SIZE 512

void* ipc_connect_client();
void  ipc_close();

void* ipc_connect_server();
