#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

static const char* BackingFileName = "sym_server_shm";
static const int BackingFileSize = 512;

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct JobRequestBuffer {
    int fd;            // File descriptor
    int cmd;           // Job command requested by the client
    int response;      // Response from the server
    int status;        // Flag indicating which stage the job is at
} JobRequestBuffer_t;

int main() {
	// Open the backing file
	int fd = shm_open(BackingFileName, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		printf("Failed to open the backing file\n");
		return -1;
	}

	// Access the shared memory
	void* shared_memory =
		mmap(NULL, BackingFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	if (shared_memory == (void*)-1) {
		printf("Failed to mmap shared memory\n");
		shm_unlink(BackingFileName);
		return -1;
	}

	JobRequestBuffer_t job_buffer;

	printf("Enter the cmd (int): ");
	scanf("%i", &job_buffer.cmd);
	printf("\n");

	job_buffer.status = JOB_REQUESTED;

	// Write the job into the shared memory
	memcpy(shared_memory, &job_buffer, sizeof(JobRequestBuffer_t));

	// Wait for the server response
	JobRequestBuffer_t* live_buffer = (JobRequestBuffer_t*)shared_memory;
	
	while (live_buffer->status != JOB_COMPLETED) {
    	continue;
  	}

	printf("Server response: %i\n", live_buffer->response);

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);

	return 0;
}
