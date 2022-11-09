#include "common.h"

#ifndef INDEPENDENT_CLIENT
#include "ipc.h"

void stress_test(int iterations, void* shared_memory) {
	clock_t start, end;
	double cpu_time_used = 0;

	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

	int status_idx = 0;
	while (job_buffer->status[status_idx] != JOB_NO_REQUEST){
		status_idx++;
		if (status_idx == MAX_CLIENT_PER_BUF){
			fprintf(stderr, "Fail to request a buffer\n");
			exit(1);
		}
	}

	job_buffer->status[status_idx] = BUF_IN_USE;
	fprintf(stderr, "buffer status idx is %d\n", status_idx);

	int command = 1; // ksys_write
	job_buffer->cmd = command; // set the request command

	// Start the performance timer
	start = clock();

	for (int i = 0; i < iterations; ++i) {
		// Indicate that the job was requested
		job_buffer->status[status_idx] = JOB_REQUESTED;

		// Wait for the job to be completed
		while (job_buffer->status[status_idx] != JOB_COMPLETED) {
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
	// Access the shared memory
	void* shared_memory = ipc_connect_client();

	if (shared_memory == NULL) {
		return -1;
	}

	// Run the stress test
	stress_test(iterations, shared_memory);

	// Cleanup

#else
	stress_test(iterations);
#endif

	return 0;
}
