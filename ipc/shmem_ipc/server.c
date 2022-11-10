#include "common.h"
#include "ipc.h"

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

	void* shared_memory = ipc_connect_server();

	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;

#ifdef DO_WORK
    FILE* log = fopen("/dev/null", "w");
    int logfd = fileno(log);
#endif

#ifdef ELEVATED
    ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", ksys_write);

    sym_elevate();
#endif

	job_buffer->status = JOB_COMPLETED;

	// Begin stress testing
	for (int i = 0; i < iterations; ++i) {
		// Wait until we get the job
        wait_for_job_request(job_buffer);

#ifdef DO_WORK
    #ifdef ELEVATED
        if (i % 20 == 0) {
			(void) !write(logfd, "ultra_test", 10);
		} else {
			(void) !ksys_write(logfd, "ultra_test", 10);
		}
    #else
        (void) !write(logfd, "ultra_test", 10);
    #endif
#endif

        // Updating the job status flag
		//mark_job_completed(job_buffer);
		__sync_bool_compare_and_swap(&job_buffer->status, JOB_REQUESTED, JOB_COMPLETED);
	}

#ifdef ELEVATED
    sym_lower();
#endif

#ifdef DO_WORK
    close(logfd);
#endif

	// Cleanup
	ipc_close();

	return 0;
}
