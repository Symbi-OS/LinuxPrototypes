#include "common.h"
#include "ipc.h"

#define LOG_FILE "/dev/null"

int64_t calc_average(int64_t* buffer, uint64_t count) {
	int64_t sum = 0;
	for (uint64_t i = 0; i < count; i++) {
		sum += buffer[i];
	}
	return sum / count;
}

#ifndef INDEPENDENT_CLIENT
int open_log_file(JobRequestBuffer_t* job_buffer) {
	job_buffer->cmd = CMD_OPEN;
	job_buffer->buffer_len = strlen(LOG_FILE);
	memcpy(job_buffer->buffer, LOG_FILE, job_buffer->buffer_len);
	job_buffer->arg1 = O_WRONLY | O_APPEND | O_CREAT;
	job_buffer->arg2 = 0644;

	// Indicate that the job was requested
	submit_job_request(job_buffer);

	// Wait for the job to be completed
	wait_for_job_completion(job_buffer);

	return job_buffer->response;
}

void close_log_file(JobRequestBuffer_t* job_buffer, int logfd) {
	job_buffer->cmd = CMD_CLOSE;
	job_buffer->arg1 = logfd;

	// Indicate that the job was requested
	submit_job_request(job_buffer);

	// Wait for the job to be completed
	wait_for_job_completion(job_buffer);
}
#endif

void stress_test(int iterations, JobRequestBuffer_t* job_buffer) {
	(void)job_buffer;

#ifndef INDEPENDENT_CLIENT
	// Tell the server to open a file
	int logfd = open_log_file(job_buffer);

	// Prepare the buffer for 'write' operation
	job_buffer->cmd = CMD_WRITE; // set the request command
	job_buffer->arg1 = logfd;
#else
	int logfd = open(LOG_FILE, O_WRONLY | O_APPEND | O_CREAT, 0644);
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

		register int count = 20000;
		while (count) {
			asm volatile ("nop");
			count --;
		}
		// Stop the inner performance timer
    	clock_gettime(CLOCK_MONOTONIC, &innerTimeEnd);

		innerTimesInNs[i] = innerTimeEnd.tv_nsec - innerTimeStart.tv_nsec;
		if (innerTimesInNs[i] < 0 && i > 0) {
			innerTimesInNs[i] = innerTimesInNs[i - 1];
		}
	}

#ifndef INDEPENDENT_CLIENT
	// Indicating that the client is done
	job_buffer->status = JOB_NO_REQUEST;
#endif

	// Stop the outer performance timer
    clock_gettime(CLOCK_MONOTONIC, &outerTimeEnd);

#ifdef INDEPENDENT_CLIENT
	// Close the log file handle
	close(logfd);
#else
	close_log_file(job_buffer, logfd);
#endif

	// Print the results
	double cpu_time_used = ((double)outerTimeEnd.tv_sec + 1.0e-9*outerTimeEnd.tv_nsec) -
						   ((double)outerTimeStart.tv_sec + 1.0e-9*outerTimeStart.tv_nsec);

  	fprintf(stderr, "%f\n", cpu_time_used);
	printf("Time used: %f seconds\n", cpu_time_used);
	printf("Throughput: %d req per second\n", (int)(iterations/cpu_time_used));
	// Calculate average latency for each iteration
	//int64_t avgIterationLatency = calc_average(innerTimesInNs, iterations);
	//fprintf(stderr, "%ld\n", avgIterationLatency);

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

	return 0;
}
