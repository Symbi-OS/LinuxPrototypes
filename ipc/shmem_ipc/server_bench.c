#include "common.h"
#include "ipc.h"

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

	void* shared_memory = ipc_connect_server();

	// Create a file to write to
	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);

	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 1;

	// Begin stress testing
	for (int i = 0; i < iterations; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
        	continue;
        }

#ifdef DO_WORK_WRITE
		// Process the requested command
		switch (job_buffer->cmd) {
		case 1: {
			write(logfd, "ksys_write\r", 11);
			break;
		}
		default: break;
		}

		// Writing in a response
		job_buffer->response = resp;
		++resp;
#endif

        // Updating the job status flag
		job_buffer->status = JOB_COMPLETED;
	}

	// Close the log file
	fclose(log);

	// Cleanup
	ipc_close();

	return 0;
}

