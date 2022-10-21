#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "LINF/sym_all.h"

#define STRESS_TEST_ITERATIONS 200000

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

#ifdef ELEVATED_MODE
typedef int(*ksys_write_t)(unsigned int fd, const char *buf, size_t count);

static ksys_write_t my_ksys_write = NULL;
#endif

int main() {
#ifdef ELEVATED_MODE
	// Init symbiote library and kallsymlib
	sym_lib_init();
#endif

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

#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
#endif

	// Create a file to write to
	FILE* log = fopen("ksys_write.log", "w");
	int logfd = fileno(log);

	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 1;

#ifdef ELEVATED_MODE
	sym_elevate();
#endif

	// Begin stress testing
	for (int i = 0; i < STRESS_TEST_ITERATIONS; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
        	continue;
        }

        // Process the requested command
		switch (job_buffer->cmd) {
		case 1: {
#ifdef ELEVATED_MODE
			my_ksys_write(logfd, "ksys_write\r", 11);
#else
			write(logfd, "ksys_write\r", 11);
#endif
			break;
		}
		default: break;
		}

		// Writing in a response
		job_buffer->response = resp;
		++resp;

        // Updating the job status flag
		job_buffer->status = JOB_COMPLETED;
	}

#ifdef ELEVATED_MODE
	sym_lower();
#endif

	// Close the log file
	fclose(log);

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);
	shm_unlink(BackingFileName);

	return 0;
}

