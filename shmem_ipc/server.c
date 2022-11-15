#include "common.h"
#include "ipc.h"

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int ITERATIONS;
int NUM_CLIENTS = 1;

void workspace_thread(workspace_t* workspace){

	// Prepare the job buffer

	#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
	sym_elevate();
	#endif
	int resp = 1;

	// Create a file to write to
	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);
	register int idx = 0;

	// Begin stress testing
	for (int i = 0; i < ITERATIONS*NUM_CLIENTS; ++i) {
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
		workspace->job_buffers[idx].response = resp;
		++resp;

        // Updating the job status flag
		workspace->job_buffers[idx].status = JOB_COMPLETED;
	}

	#ifdef ELEVATED_MODE
	sym_lower();
	#endif

	// Close the log file
	fclose(log);
}



int main(int argc, char** argv) {
	

	if (argc < 1){
		printf("Usage: ./<server binary> <iterations>\n");
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