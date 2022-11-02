#include "common.h"
#include <LINF/sym_all.h>


void getppid_loop(int iterations, int independent, job_buffer_t *work_job_buffer) {

	#ifdef ELEVATED
	getppid_t getppid_elevated = (getppid_t)sym_get_fn_address((char*)"__x64_sys_getppid");
	sym_elevate();
	#endif

	START_CLOCK();
	for (int i = 0; i < iterations; ++i) {
		if (independent){
			#ifdef ELEVATED
			getppid_elevated();
			#else
			getppid();
			#endif
		}else{
			server_getppid(work_job_buffer);
		}

	}
	STOP_CLOCK();

	double time_used = GET_DURATION();
	printf("Time used: %f\n", time_used);

	#ifdef ELEVATED
	sym_lower();
	#endif

	if (work_job_buffer != NULL){
		// free the job buffer
        work_job_buffer->status = JOB_NO_REQUEST;
    }

}


int main(int argc, char** argv) {
	UNUSED(argc);
	workspace_t *workspace = NULL;
	job_buffer_t *job_buffer = NULL;

	if (argc < 3){
		printf("Usage: ./<client binary> <num iterations> <if_independent>\n");
		return 1;
	}

	int iterations = atoi(argv[1]);
	int independent = atoi(argv[2]);

	if (!independent){ // need to connect to the server
		workspace = connect_server();
		if (workspace == (void *)-1){
        	printf("Fail to connect to server...\n");
        	return -1;
    	}

		printf("workspace at %p\n", workspace);

		job_buffer = get_job_buffer(workspace);
		if (job_buffer == (void *)-1) {
			printf("Fail to request working job_buffer...\n");
			return 1;
		}

		printf("job buffer at %p\n", job_buffer);
	}

	//start the test
	getppid_loop(iterations, independent, job_buffer);

	//disconnect
	if (!independent){
		munmap(workspace, BACKING_FILE_SIZE);
	}
	return 0;
}

