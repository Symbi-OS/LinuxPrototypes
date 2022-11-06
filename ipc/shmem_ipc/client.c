#include "common.h"
#include <LINF/sym_all.h>
#include "time.h"

#define WRITE_BUF "BlackMagic\n"
#define WRITE_LOC "tmp.txt"

void write_loop_independent(int iterations){

	clock_t start, end;

	#ifdef ELEVATED
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	sym_elevate();
	#endif
	FILE * fp;
	int fd;

	fp = fopen(WRITE_LOC, "w");
	fd = fileno(fp);
	start = clock();
	for (int i = 0; i < iterations; i++){
		#ifdef ELEVATED
		printf("heree\n");
		if (i % 20 == 0) {
           		write(fd, "ksys_write\r", 11);
        	} else {
			ksys_write(fd, "ksys_write\r", 11);
        	}	
		#else
		write(fd, "ksys_write\r", 11);
		#endif
	}
	end = clock();


	close(fd);
	#ifdef ELEVATED
	sym_lower();
	#endif

	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", cpu_time_used);

	return;
}
	
	
	
void write_loop_server(int iterations, job_buffer_t * work_job_buffer){
	clock_t start, end;

	work_job_buffer->cmd = CMD_WRITE;
	work_job_buffer->buf_len = strlen(WRITE_BUF);
	strcpy(work_job_buffer->buf, WRITE_BUF);

	start=clock();
	for (int i = 0; i < iterations; i++){
		work_job_buffer->status = JOB_REQUESTED;

		// Wait for the job to be completed
		while (work_job_buffer->status != JOB_COMPLETED) {
			continue;
		}
		//server_write(work_job_buffer, WRITE_BUF, strlen(WRITE_BUF));
	}
	end=clock();
	work_job_buffer->status = JOB_NO_REQUEST;

	double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f\n", cpu_time_used);
    return;
}


int main(int argc, char** argv) {
	UNUSED(argc);
	

	if (argc < 3){
		printf("Usage: ./<client binary> <num iterations> <if_independent>\n");
		return 1;
	}

	int iterations = atoi(argv[1]);
	int independent = atoi(argv[2]);

	job_buffer_t *job_buffer = NULL;

	if (!independent){ // need to connect to the server
		job_buffer = get_job_buffer();
		if (job_buffer == (void *)-1) {
			printf("Fail to request working job_buffer...\n");
			return 1;
		}

		write_loop_server(iterations, job_buffer);
	}else{
		write_loop_independent(iterations);
	}

	

	//disconnect
	if (job_buffer != NULL){
		munmap(job_buffer, sizeof(job_buffer));
	}
	return 0;
}

