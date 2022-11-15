#include "common.h"
#include "ipc.h"

int64_t calc_average(int64_t* buffer, uint64_t count) {
	int64_t sum = 0;
	for (uint64_t i = 0; i < count; i++) {
		sum += buffer[i];
	}
	return sum / count;
}

void stress_test(int iterations, JobRequestBuffer_t* job_buffer) {
	(void)job_buffer;

#ifndef INDEPENDENT_CLIENT
	job_buffer->cmd = CMD_WRITE; // set the request command
#else
	FILE* log = fopen("/dev/null", "w");
    int logfd = fileno(log);
#endif
	// Create performance timers
	struct timespec outerTimeStart={0,0}, outerTimeEnd={0,0},
					innerTimeStart={0,0}, innerTimeEnd={0,0};

	// Buffer to hold inner loop times
	int64_t* innerTimesInNs = (int64_t*)malloc(iterations * sizeof(int64_t));

	// Start outer performance timer
    clock_gettime(CLOCK_MONOTONIC, &outerTimeStart);

	for (int i = 0; i < iterations; ++i) {
		// Start inner performance timer
    	clock_gettime(CLOCK_MONOTONIC, &innerTimeStart);

#ifndef INDEPENDENT_CLIENT
        // Indicate that the job was requested
		submit_job_request(job_buffer);

        // Wait for the job to be completed
		wait_for_job_completion(job_buffer);
#else
        (void) !write(logfd, "ksys_write\r", 11);
#endif

		// Stop the inner performance timer
    	clock_gettime(CLOCK_MONOTONIC, &innerTimeEnd);

		innerTimesInNs[i] = innerTimeEnd.tv_nsec - innerTimeStart.tv_nsec;
	}

	// Stop the outer performance timer
    clock_gettime(CLOCK_MONOTONIC, &outerTimeEnd);

#ifdef INDEPENDENT_CLIENT
	// Close the log file handle
	close(logfd);
#endif

	// Print the results
	double cpu_time_used = ((double)outerTimeEnd.tv_sec + 1.0e-9*outerTimeEnd.tv_nsec) -
						   ((double)outerTimeStart.tv_sec + 1.0e-9*outerTimeStart.tv_nsec);
  	fprintf(stderr, "%f ", cpu_time_used);

	// Calculate average latency for each iteration
	int64_t avgIterationLatency = calc_average(innerTimesInNs, iterations);
	fprintf(stderr, "%ld\n", avgIterationLatency);

	// Release the timer buffer
	free(innerTimesInNs);
}

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

#ifndef INDEPENDENT_CLIENT
    // Access the shared memory
	JobRequestBuffer_t* job_buffer = ipc_get_job_buffer();
	if (job_buffer == NULL) {
		fprintf(stderr, "Fail to request working job_buffer...\n");
		return -1;
	}
#else
    JobRequestBuffer_t* job_buffer = 0;
#endif

	// Run the stress test
	stress_test(iterations, job_buffer);

#ifndef INDEPENDENT_CLIENT
    // Cleanup
	ipc_close();
#endif

	return 0;
}
