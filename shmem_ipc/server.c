#include "common.h"
#include "ipc.h"
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <errno.h>

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int ITERATIONS;
int target_logfd = -1;
int NUM_CLIENTS = 1;

void workspace_thread(workspace_t* workspace){

	// Prepare the job buffer

#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
	sym_elevate();
#endif

	// Begin stress testing (+2 is necessary for open and close calls)
	for (int i = 0; i < ITERATIONS + 2; ++i) {
		// Wait until we get the job
        while (workspace->job_buffers[idx].status != JOB_REQUESTED) {
			idx++;
			if (idx==NUM_CLIENTS){
				idx = 0;
			}
        	continue;
        }

        // Process the requested command
		switch (workspace->job_buffers[idx].cmd) {
		case CMD_OPEN: {
			job_buffer->response = open(job_buffer->buffer, job_buffer->arg1, job_buffer->arg2);
			break;
		}
		case CMD_CLOSE: {
			job_buffer->response = close(job_buffer->arg1);
			break;
		}
		case CMD_WRITE: {
#ifdef ELEVATED_MODE
			if (i % 20 == 0) {
				(void) !write(job_buffer->arg1, "ksys_write\r", 11);
			} else {
				(void) !my_ksys_write(job_buffer->arg1, "ksys_write\r", 11);
			}
#else
			(void) !write(job_buffer->arg1, "ksys_write\r", 11);
#endif
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
	}

#ifdef ELEVATED_MODE
	sym_lower();
#endif

	pthread_exit(NULL);
}



int main(int argc, char** argv) {
	

	if (argc < 1){
		printf("Usage: ./<server binary> <iterations> [optional]<nthreads>\n");
	}else if(argc == 2){
		ITERATIONS = atoi(argv[1]);
	}else if(argc == 3){
		ITERATIONS = atoi(argv[1]);
		NUM_CLIENTS = atoi(argv[2]);
	}

	workspace_t* workspace = ipc_connect_server();

	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }

	workspace_thread(workspace);

	ipc_close();
	return 0;
}