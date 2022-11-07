#include "common.h"
#include "ipc.h"

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

#ifdef ELEVATED_MODE
	// Init symbiote library and kallsymlib
	sym_lib_init();
#endif

	void* shared_memory = ipc_connect_server();

#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
#endif

	// Create a file to write to
	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);

	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 1;

#ifdef ELEVATED_MODE
	sym_elevate();
#endif

	// Begin stress testing
	for (int i = 0; i < iterations; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
        	continue;
        }

        // Process the requested command
		switch (job_buffer->cmd) {
		case 1: {
#ifdef ELEVATED_MODE
			if (i % 20 == 0) {
				write(logfd, "ksys_write\r", 11);
			} else {
				my_ksys_write(logfd, "ksys_write\r", 11);
			}
#else
			write(logfd, "ksys_write\r", 11);
#endif
			break;
		}
		default: break;
		}

		// Writing in a response
		job_buffer->response = resp;
		++resp;

        // Updating the job status flag
		job_buffer->status = JOB_COMPLETED;
	}

#ifdef ELEVATED_MODE
	sym_lower();
#endif

	// Close the log file
	fclose(log);

	// Cleanup
	ipc_close();

	return 0;
}

