#include "common.h"
#include "ipc.h"

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int main(int argc, char** argv) {
	(void)argc;
	int iterations = atoi(argv[1]);

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

#ifdef ELEVATED_MODE
	sym_elevate();
#endif

	int i = 0; 
	int current_idx = 0;
	// Begin stress testing
	while (1) {
		// Wait until we get the job
		if (job_buffer->status[current_idx] != JOB_REQUESTED){
			current_idx++;
			if (current_idx == MAX_CLIENT_PER_BUF){
				current_idx = 0;
			}
			if (i== (iterations*MAX_CLIENT_PER_BUF)){
				#ifdef ELEVATED_MODE
				sym_lower();
				#endif
				fclose(log);
				// Cleanup
				ipc_close();
				return 0;
			}
		}else{
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
			
			i++;
			// Updating the job status flag
			job_buffer->status[current_idx] = JOB_COMPLETED;
		}
	}
}


