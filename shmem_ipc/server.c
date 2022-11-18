#include "common.h"
#include "ipc.h"
#include <pthread.h>

int ITERATIONS;
int target_logfd = -1;

void* job_buffer_thread(void* current_job_buffer){
	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)current_job_buffer;

	// Begin stress testing (+2 is necessary for open and close calls)
	for (int i = 0; i < ITERATIONS + 2; ++i) {
		// Wait until we get the job
        wait_for_job_request(job_buffer);

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
			(void) !write(job_buffer->arg1, "ksys_write\r", 11);
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
	}

	pthread_exit(NULL);
}

int main(int argc, char** argv) {
	int num_threads = 1;

	if (argc < 1){
		printf("Usage: ./<server binary> <iterations> [optional]<nthreads>\n");
	}else if(argc == 2){
		ITERATIONS = atoi(argv[1]);
	}else if(argc == 3){
		ITERATIONS = atoi(argv[1]);
		num_threads = atoi(argv[2]);
	}

	workspace_t* workspace = ipc_connect_server();

	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }

	pthread_t tid[num_threads];
	for (int j = 0; j < num_threads; j++){
		int err = pthread_create(&(tid[j]), NULL, &job_buffer_thread, &workspace->job_buffers[j]);
		if (err != 0) printf("can't create thread :[%s]", strerror(err));
	}

	for (int j = 0; j < num_threads; j++){
		pthread_join(tid[j], NULL);
	}

	ipc_close();
	return 0;
}