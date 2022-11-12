#include "common.h"
#include "ipc.h"

void stress_test(int iterations, void* shared_memory) {
    FILE* log = fopen("/dev/null", "w");
    int logfd = fileno(log);

#ifndef INDEPENDENT_CLIENT
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

	//int command = 1; // ksys_write
	//job_buffer->cmd = command; // set the request command
#endif

	// Start the performance timer
	struct timespec tstart={0,0}, tend={0,0};
    clock_gettime(CLOCK_MONOTONIC, &tstart);

	//struct timespec in_loop_tstart={0,0}, in_loop_tend={0,0};
	//double* iteration_latencies = (double*)malloc(iterations * sizeof(double));

	for (int i = 0; i < iterations; ++i) {
		//clock_gettime(CLOCK_MONOTONIC, &in_loop_tstart);

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
		/*
		clock_gettime(CLOCK_REALTIME, &in_loop_tend);
		long seconds = in_loop_tend.tv_sec - in_loop_tstart.tv_sec;
   	 	long nanoseconds = in_loop_tend.tv_nsec - in_loop_tstart.tv_nsec;
    	double elapsed = seconds + nanoseconds * 1e-9;
		iteration_latencies[i] = elapsed;*/
	}

	// Stop the performance timer
    clock_gettime(CLOCK_MONOTONIC, &tend);

    close(logfd);

	// Print the results
	double cpu_time_used = ((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec);
  	printf("Time used: %f\n", cpu_time_used);
  	
	/*for (int i = 0; i < iterations; i++) {
		printf("Time used: %f\n", iteration_latencies[i]);
	}
	free(iteration_latencies);*/
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
