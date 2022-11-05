#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

static const char* BackingFileName = "sym_server_shm";
static const int BackingFileSize = 512;

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct JobRequestBuffer {
    int fd;               // File descriptor
    int cmd;              // Job command requested by the client
    int response;         // Response from the server
    volatile int status;  // Flag indicating which stage the job is at
} JobRequestBuffer_t;

void stress_test(int iterations, void* shared_memory) {
	clock_t start, end;
	double cpu_time_used = 0;

	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

	int command = 1; // ksys_write
	job_buffer->cmd = command; // set the request command

	// Start the performance timer
	start = clock();

	for (int i = 0; i < iterations; ++i) {
		// Indicate that the job was requested
		job_buffer->status = JOB_REQUESTED;

		// Wait for the job to be completed
		while (job_buffer->status != JOB_COMPLETED) {
			continue;
		}

		// Hand the response... (job_buffer->response)
	}

	// Stop the performance timer
	end = clock();
	
	// Print the results
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  	printf("Time used: %f\n", cpu_time_used);
}

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

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

	// Run the stress test
	stress_test(iterations, shared_memory);

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);

	return 0;
}
