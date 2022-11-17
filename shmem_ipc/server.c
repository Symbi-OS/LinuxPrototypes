#include "common.h"
#include "ipc.h"

int ITERATIONS;
int target_logfd = -1;
int NUM_CLIENTS = 1;

void workspace_thread(workspace_t* workspace){
	int idx = 0;

	// Begin stress testing (+2 is necessary for open and close calls)
	register int idx = 0;
	for (int i = 0; i < (ITERATIONS + 2)*NUM_CLIENTS; ++i) {
		// Wait until we get the job
        while (workspace->job_buffers[idx].status != JOB_REQUESTED) {
			idx++;
			if (idx==NUM_CLIENTS){
				idx = 0;
			}
        	continue;
        }

		JobRequestBuffer_t* job_buffer = &workspace->job_buffers[idx];

        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_OPEN: {
			job_buffer->response = open(job_buffer->buffer, job_buffer->arg1, job_buffer->arg2);
			break;
		}
		case CMD_CLOSE: {
			job_buffer->response = close(job_buffer->arg1);
			break;
		}
		case CMD_WRITE: {
			job_buffer->response = write(job_buffer->arg1, "ksys_write\r", 11);
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
	}
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