#include "common.h"
#include "ipc.h"

void stress_test(int iterations, void* shared_memory) {
	clock_t start, end;
	double cpu_time_used = 0;

    FILE* log = fopen("log", "w");
    int logfd = fileno(log);

#ifndef INDEPENDENT_CLIENT
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

	int command = 1; // ksys_write
	job_buffer->cmd = command; // set the request command
#endif

	// Start the performance timer
	start = clock();

    volatile int i;
	for (i = 0; i < iterations; ++i) {
#ifndef INDEPENDENT_CLIENT
        // Indicate that the job was requested
		submit_job_request(job_buffer);

#ifdef DO_WORK
        (void) !write(logfd, "ultra_test", 10);
#endif

        // Wait for the job to be completed
		wait_for_job_completion(job_buffer);
#else
        (void) !write(logfd, "ultra_test", 10);
#endif
	}

	// Stop the performance timer
	end = clock();

    close(logfd);

	// Print the results
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  	printf("Time used: %f\n", cpu_time_used);
}

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

#ifndef INDEPENDENT_CLIENT
    void* shared_memory = ipc_connect_client();
#else
    void* shared_memory = 0;
#endif

	// Run the stress test
	stress_test(iterations, shared_memory);

#ifndef INDEPENDENT_CLIENT
    // Cleanup
	ipc_close();
#endif

	return 0;
}