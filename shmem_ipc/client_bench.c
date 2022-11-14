#include "common.h"
#include "ipc.h"

static int logfd = 0;

void do_work_write() {
	write(logfd, "ksys_write\r", 11);
}

void do_work_nop() {
	volatile int j;
	for (j = 0; j < 1000; j++);
}

void stress_test(int iterations, void* shared_memory) {
	clock_t start, end;
	double cpu_time_used = 0;

#ifdef IPC_COMM
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

	int command = 1; // ksys_write
	job_buffer->cmd = command; // set the request command
#endif

#ifdef DO_WORK_WRITE
	FILE* log = fopen("stress_test_log", "w");
    logfd = fileno(log);
#endif

	// Start the performance timer
	start = clock();

	for (int i = 0; i < iterations; ++i) {
#ifdef IPC_COMM
		// Indicate that the job was requested
		job_buffer->status = JOB_REQUESTED;
#endif

#ifdef DO_WORK_WRITE
		do_work_write();
#else
		//do_work_nop();
#endif

#ifdef IPC_COMM
		// Wait for the job to be completed
		while (job_buffer->status != JOB_COMPLETED) {
			continue;
		}
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

#ifdef IPC_COMM
	// Access the shared memory
	void* shared_memory = ipc_connect_client();
#else
	void* shared_memory = NULL;
#endif

	// Run the stress test
	stress_test(iterations, shared_memory);

#ifdef IPC_COMM
	// Cleanup
	ipc_close();
#endif

	return 0;
}
