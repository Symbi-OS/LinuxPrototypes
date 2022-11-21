#include "common.h"
#include "ipc.h"
#include <pthread.h>

int target_logfd = -1;
static uint8_t bShouldExit = 0;
pthread_spinlock_t locks[MAX_JOB_BUFFERS];
static uint8_t bShouldExit = 0;

void workspace_thread(workspace_t* workspace){
	// Begin stress testing (+2 is necessary for open and close calls)
	register int idx = 0;
	while (!bShouldExit) {
		// Wait until we get the job
        while (workspace->job_buffers[idx].status != JOB_REQUESTED) {
			idx++;
			if (idx==NUM_CLIENTS){
				idx = 0;
	workspace_t *workspace = (workspace_t *) ws;
	register int idx = 0;
	JobRequestBuffer_t* job_buffer;

	while (!bShouldExit) {
		// obtain lock to update next buffer
		
        while (1) {
			if (pthread_spin_trylock(&locks[idx]) == 0){
				if (workspace->job_buffers[idx].status == JOB_REQUESTED){
					goto JOB_FOUND;
				}else{
					pthread_spin_unlock(&locks[idx]);
					idx++;
					if (idx==MAX_JOB_BUFFERS){
						idx = 0;
					}
				}
			}else{
				idx++;
				if (idx==MAX_JOB_BUFFERS){
					idx = 0;
				}
			}
        }
		
		JOB_FOUND:
		job_buffer = &workspace->job_buffers[idx];
		job_buffer->status = JOB_PICKEDUP;
		// Check if the server has been killed
		if (job_buffer->cmd == CMD_KILL_SERVER) {
			bShouldExit = 1;
			mark_job_completed(job_buffer);
			break;
		}


		pthread_spin_unlock(&lock);

		// Check if the server has been killed
		if (job_buffer->cmd == CMD_KILL_SERVER) {
			bShouldExit = 1;
			mark_job_completed(job_buffer);
			break;
		}

        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_OPEN: {
			job_buffer->response = open(job_buffer->buffer, job_buffer->arg1);
			break;
		}
		case CMD_CLOSE: {
			job_buffer->response = close(job_buffer->arg1);
			break;
		}
		case CMD_WRITE: {
			job_buffer->response = write(job_buffer->arg1, job_buffer->buffer, job_buffer->buffer_len);
			break;
		}
		case CMD_READ: {
			job_buffer->response = read(job_buffer->arg1, job_buffer->buffer, job_buffer->buffer_len);
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
		pthread_spin_unlock(&locks[idx]);
	}

	pthread_exit(NULL);
}



int main(int argc, char** argv) {

	int num_threads = 1;

	if (argc < 1){
		printf("Usage: ./<server binary> [optional]<nthreads>\n");
	} else if (argc == 2) {
		NUM_CLIENTS = atoi(argv[1]);
	}


	workspace_t* workspace = ipc_connect_server();


	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }


	
	pthread_t tid[num_threads];

	for (int j = 0; j < MAX_JOB_BUFFERS; j++){
		if (pthread_spin_init(&(locks[j]), PTHREAD_PROCESS_PRIVATE) != 0) {
			printf("\n mutex init has failed\n");
			return 1;
		}
	}

	for (int j = 0; j < num_threads; j++){
		int err = pthread_create(&(tid[j]), NULL, &workspace_thread, workspace);
		printf("Thread starting at: %p\n", &tid[j]);
		if (err != 0) printf("can't create thread :[%s]", strerror(err));
	}

	for (int j = 0; j < num_threads; j++){
		pthread_join(tid[j], NULL);
	}

	ipc_close();
	return 0;
}