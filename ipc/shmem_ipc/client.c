#include "common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

static const char* BackingFileName = "sym_server_shm";
static const int BackingFileSize = 512;

void stress_test_shared_memory_client(int iterations, void* shared_memory) {
	clock_t start, end;
	double cpu_time_used = 0;

	volatile JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

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

int stress_test_independent_client(int iteration_count) {
	FILE* log = fopen("stress_test_independent_client_log", "w");
    int logfd = fileno(log);

	clock_t start, end;
    double cpu_time_used = 0;

#ifdef ELEVATED_MODE
    // Get the address of ksys_write
    ksys_write_t my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");

    // Elevate
    sym_elevate();
#endif

	// Start performance timer
    start = clock();

    // Begin stress testing
    for (int i = 0; i < iteration_count; ++i) {
#ifdef ELEVATED_MODE
        my_ksys_write(logfd, "ksys_write\r", 11);
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

	return 0;
}

int start_shared_memory_test(int iteration_count) {
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
    stress_test_shared_memory_client(iteration_count, shared_memory);

    // Cleanup
    munmap(shared_memory, BackingFileSize);
    close(fd);	
	
	return 0;
}

int main(int argc, char** argv) {
	UNUSED_PARAM(argc)

	// Get the number of stress test iterations from the command lien
	int iteration_count = atoi(argv[1]);
	int result;

#ifdef INDEPENDENT_CLIENT
    result = stress_test_independent_client(iteration_count);
#else
	result = start_shared_memory_test(iteration_count);
#endif

	return result;
}
