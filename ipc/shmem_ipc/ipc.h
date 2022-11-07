#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct JobRequestBuffer {
    int fd;               // File descriptor
    int cmd;              // Job command requested by the client
    int response;         // Response from the server
	char buffer[128];	  // Command buffer
	int buffer_len;		  // Commabd buffer length
    volatile int status;  // Flag indicating which stage the job is at
} JobRequestBuffer_t;

#define SHMEM_REGION_SIZE 512

void* ipc_connect_client();
void  ipc_close_client();
