#include "common.h"

#ifndef INDEPENDENT_CLIENT
#include "ipc.h"

static const char* BackingFileName = "sym_server_shm";
static const int BackingFileSize = 512;

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
#else
void stress_test(int iterations) {
	clock_t start, end;
    double cpu_time_used = 0;

    FILE* log = fopen("stress_test_log", "w");
    int logfd = fileno(log);

#ifdef ELEVATED_MODE
    // Init symbiote library and kallsymlib
    sym_lib_init();

    // Get the address of ksys_write
    ksys_write_t my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");

    // Elevate
    sym_elevate();
#endif

    // Start performance timer
    start = clock();

    // Begin stress testing
    for (int i = 0; i < iterations; ++i) {
#ifdef ELEVATED_MODE
        if (i % 20 == 0) {
            write(logfd, "ksys_write\r", 11);
        } else {
            my_ksys_write(logfd, "ksys_write\r", 11);
        }
#else
        write(logfd, "ksys_write\r", 11);
#endif
    }

    // Stop performance timer
    end = clock();

#ifdef ELEVATED_MODE
    sym_lower();
#endif

    // Print the results
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Time used: %f\n", cpu_time_used);

	close(logfd);
}
#endif

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

#ifndef INDEPENDENT_CLIENT
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
#else
	stress_test(iterations);
#endif

	return 0;
}
