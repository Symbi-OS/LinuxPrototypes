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
	// Create the backing file
	int fd = shm_open(
		BackingFileName,
		O_RDWR | O_CREAT,
		S_IRUSR | S_IWUSR
	);

	if (fd < 0) {
		printf("Can't open shared memory segment\n");
		shm_unlink(BackingFileName);
		return -1;
	}

	// Resize the backing file
	ftruncate(fd, BackingFileSize);

	// Get access to the shared memory
	void* shared_memory =
		mmap(NULL, BackingFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shared_memory == (void*)-1) {
		printf("Can't mmap the shared memory\n");
		shm_unlink(BackingFileName);
		return -1;
	}

	// Zero out the backing file
	memset(shared_memory, 0, BackingFileSize);

	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 5;

	for (int i = 0; i < 3; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
        	continue;
        }

        // Read the requested command
        printf("REQUESTED_CMD: %i\n", job_buffer->cmd);

		// Writing in a response
		job_buffer->response = resp;
		++resp;

        // Updating the job status flag
		job_buffer->status = JOB_COMPLETED;
	}

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);
	shm_unlink(BackingFileName);

	return 0;
}

