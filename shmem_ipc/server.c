#include "common.h"
#include "ipc.h"
#include <pthread.h>

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int ITERATIONS;

void* job_buffer_thread(void *current_job_buffer){

	// Prepare the job buffer
	JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)current_job_buffer;
	int resp = 1;

	#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
	sym_elevate();
	#endif

	// Create a file to write to
	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);

	// Begin stress testing
	for (int i = 0; i < ITERATIONS; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
        	continue;
        }

        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_WRITE: {
#ifdef ELEVATED_MODE
			if (i % 200 == 0) {
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
	pthread_exit (NULL);
}

int main(int argc, char** argv) {
	
	int num_threads = 1;

	if (argc < 1){
		printf("Usage: ./<server binary> <iterations>\n");
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