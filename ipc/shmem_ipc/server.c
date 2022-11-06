#include "common.h"
#include <LINF/sym_all.h>
#include <pthread.h>
#include "LINF/sym_all.h"

//hardcode locations
#define WRITE_BUF "BlackMagic\n"
#define WRITE_LOC "tmp.txt"

void* job_buffer_thread(void *job_buffer){
	
	job_buffer_t *request_job_buffer = job_buffer;
	//clock_t end = clock() + SERVER_TIMEOUT*CLOCKS_PER_SEC;

	//stub hardcoded variables
	FILE * fp = fopen(WRITE_LOC, "w");
	int fd = fileno(fp);
	clock_t end = clock() + SERVER_TIMEOUT * CLOCKS_PER_SEC;

	#ifdef ELEVATED
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	getppid_t getppid_elevated = (getppid_t)sym_get_fn_address((char*)"__x64_sys_getppid");
	printf("ksys_write: %p\n", ksys_write);

	sym_elevate();
	#endif

	printf("thread started at %p\n", job_buffer);
		// ever running thread
		//wait{ for request
	LOOP:	
		while (request_job_buffer->status != JOB_REQUESTED) {
			if ((request_job_buffer->status == SERVER_KILL_SIGNAL)||(clock() > end)){
				#ifdef ELEVATED
				sym_lower();
				#endif
				fclose(fp);
				pthread_exit (NULL);
			}
		}
		int i = 0;
		switch(request_job_buffer->cmd){

			case CMD_WRITE: {
				
				#ifdef ELEVATED
				i++;
				if (i % 20 == 0){
					write(fd, "ksys_write\r", 11);
				}else{
					ksys_write(fd, "ksys_write\r", 11);
				}
				#else
				write(fd, "ksys_write\r", 11);
				#endif
				break;
			}
			default:{
				break;
			}

		}
		
		request_job_buffer->status = JOB_COMPLETED;

		goto LOOP;
}

void single_job_buffer(job_buffer_t * job_buffer){
	
	job_buffer_t *request_job_buffer = job_buffer;
	//clock_t end = clock() + SERVER_TIMEOUT*CLOCKS_PER_SEC;

	//stub hardcoded variables
	//clock_t end = clock() + SERVER_TIMEOUT * CLOCKS_PER_SEC;

	#ifdef ELEVATED
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	sym_elevate();
	#endif

	printf("thread started at %p\n", job_buffer);
	// Create a file to write t
	FILE * fp = fopen("tmp.txt", "w");
    int fd = fileno(fp);
	
		// ever running thread
		//wait{ for request
	for (int j = 0; j < 200000; j++){	
		while (request_job_buffer->status != JOB_REQUESTED) {
		
			continue;
		}
        // Process the requested command
		int i = 0;
		switch(request_job_buffer->cmd){

			case CMD_WRITE: {
				
				#ifdef ELEVATED
				i++;
				if (i % 2000 == 0){
					write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				}else{
					ksys_write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				}
				#else
				write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				#endif
				request_job_buffer->response = 1;
				break;
			}

			default: {
				break;
			}

		}
		
		request_job_buffer->status = JOB_COMPLETED;

	}

	#ifdef ELEVATED
	sym_lower();
	#endif
	fclose(fp);
	return;
}


int main(int argc, char** argv) {
	UNUSED(argc);

	if (argc < 2){
		printf("Usage: ./<server binary> <num threads>\n");
		printf("currently max thread is 2\n");
	}

	int num_threads = atoi(argv[1]);

	workspace_t *workspace = server_init();

	printf("server started at %p\n", workspace);

    if (workspace == (void *)-1){
        printf("Fail to intialize server...\n");
    }

	int DEBUG = 1;

	if (DEBUG){
		single_job_buffer(&workspace->job_buffers[0]);
	}else{
		//use pthread to organize all threads
		pthread_t tid[num_threads];
		for (int j = 0; j < num_threads; j++){
			int err = pthread_create(&(tid[j]), NULL, &job_buffer_thread, &workspace->job_buffers[j]);
			if (err != 0) printf("can't create thread :[%s]", strerror(err));
		}

		for (int j = 0; j < num_threads; j++){
			pthread_join(tid[j], NULL);
		}
	}

	
    //close up the server
    munmap(workspace, BACKING_FILE_SIZE);
    shm_unlink(BACKING_FILE);

	return 0;
}
