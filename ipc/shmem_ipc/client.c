#include "common.h"
#include <LINF/sym_all.h>

#define WRITE_BUF "BlackMagic\n"
#define WRITE_LOC "tmp.txt"

void getppid_loop(int iterations, job_buffer_t *work_job_buffer) {

	START_CLOCK();
	#ifdef ELEVATED
	getppid_t getppid_elevated = (getppid_t)sym_get_fn_address((char*)"__x64_sys_getppid");
	sym_elevate();
	#endif
	
	if (iterations == 0){
		sleep(5);
		return;
	}


	for (int i = 0; i < iterations; ++i) {
		if (work_job_buffer==NULL){
			//perform independent write
			#ifdef ELEVATED
			getppid_elevated();
			#else
			getppid();
			#endif
		}else{
			server_getppid(work_job_buffer);
		}

	}
	

	#ifdef ELEVATED
	sym_lower();
	#endif

	if (work_job_buffer != NULL){
		// free the job buffer
        work_job_buffer->status = JOB_NO_REQUEST;
    }

	STOP_CLOCK();
	double time_used = GET_DURATION();
	printf("Time used: %f\n", time_used);
}

void write_loop(int iterations, job_buffer_t *work_job_buffer){

	START_CLOCK();
	if (work_job_buffer==NULL){
		#ifdef ELEVATED
		ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
		sym_elevate();
		#endif
		FILE * fp;
    	int fd;

		fp = fopen(WRITE_LOC, "a");
		fd = fileno(fp);


		for (int i = 0; i < iterations; i++){
			#ifdef ELEVATED
			ksys_write(fd, WRITE_BUF, strlen(WRITE_BUF));
			#else
			write(fd, WRITE_BUF, strlen(WRITE_BUF));
			#endif
		}

		close(fd);
		#ifdef ELEVATED
		sym_lower();
		#endif
	}else{

		for (int i = 0; i < iterations; i++){
			
			server_write(work_job_buffer, WRITE_LOC, WRITE_BUF, strlen(WRITE_BUF));
		}

		work_job_buffer->status = JOB_NO_REQUEST;
	}

	STOP_CLOCK();
	double time_used = GET_DURATION();
	printf("Time used: %f\n", time_used);
    return;
}


int main(int argc, char** argv) {
	UNUSED(argc);
	job_buffer_t *job_buffer = NULL;

	if (argc < 4){
		printf("Usage: ./<client binary> <num iterations> <if_independent> < write | pid >\n");
		return 1;
	}

	int iterations = atoi(argv[1]);
	int independent = atoi(argv[2]);

	if (!independent){ // need to connect to the server
		job_buffer = get_job_buffer();
		if (job_buffer == (void *)-1) {
			printf("Fail to request working job_buffer...\n");
			return 1;
		}

	}

	//start the test
	if (!strcmp(argv[3], "write")){
		write_loop(iterations, job_buffer);
	}else if (!strcmp(argv[3], "pid")){
		getppid_loop(iterations, job_buffer);
	}
	

	//disconnect
	if (job_buffer != NULL){
		munmap(job_buffer, sizeof(job_buffer));
	}
	return 0;
}

