#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include "LINF/sym_all.h"

/*
Function that initalize the server and workspace
*/
typedef int(*ksys_write_t)(unsigned int fd, const char *buf, size_t count);

void* server_init(){

	int fd = shm_open(BACKING_FILE, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

	if (fd < 0) {
		printf("Can't open shared memory segment\n");
		shm_unlink(BACKING_FILE);
		return (void*)-1;
	}

	// Resize the backing file
	ftruncate(fd, BACKING_FILE_SIZE);

	// Get access to the shared memory
	void* shared_memory_ptr =
		mmap(NULL, BACKING_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shared_memory_ptr == (void*)-1) {
		printf("Can't mmap the shared memory\n");
		shm_unlink(BACKING_FILE);
		return (void*)-1;
	}

	memset(shared_memory_ptr, 0, BACKING_FILE_SIZE);

	// points to the next available spot

	workspace_t * workspace = (workspace_t *)shared_memory_ptr;

	return workspace;
}


/*
Function that for client to get empty job_buffer to communicate
*/
job_buffer_t * get_job_buffer(workspace_t * workspace){

	for (int i = 0; i < NUM_JOB_BUFFERS; i++){
		job_buffer_t * current = &(workspace->job_buffers[i]);
		if (current->status == EMPTY_JOB_BUFFER){
			//find a free spot!
			current->status = JOB_BUFFER_IN_USE;
			return &workspace->job_buffers[i];
		}
	}

	return (void *)-1;
}

void* connect_server(){

	int fd = shm_open(BACKING_FILE, O_RDWR, S_IRUSR | S_IWUSR);

	if (fd < 0) {
		printf("Failed to connect shared memory\n");
		return (void*)-1;
	}

	// Access the shared memory
	void* shared_memory =
		mmap(NULL, BACKING_FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);


	if (shared_memory == (void*)-1) {
		printf("Failed to mmap shared memory\n");
		shm_unlink(BACKING_FILE);
		return (void*)-1;
	}

	return shared_memory;
}

void* job_buffer_thread(void *job_buffer){
	
	job_buffer_t *request_job_buffer = job_buffer;

	FILE * fp = NULL;
	int fd = -1;
	char * last_filename = "";

	#ifdef ELEVATED
	// Get the adress of ksys_write
	sym_lib_init();
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	sym_elevate();
	#endif

	request_job_buffer->status = EMPTY_JOB_BUFFER;

	while (1){ // ever running thread


		//wait for request
		while (request_job_buffer->status != REQUEST_SENT) {
			if (request_job_buffer->status == KILL_SIGNAL){
				#ifdef ELEVATED
				sym_lower();
				#endif
				pthread_exit (NULL);
			}
			continue;
		}

		request_job_buffer->status = REQUEST_RECEIVED;

		request_job_buffer->status = REQUEST_PROCESSING;

		switch(request_job_buffer->cmd){
			case 1: {
				
				if ((fd == -1) | strcmp(request_job_buffer->filename, last_filename)){
					if (fd != -1){
						close(fd);
					}
					fp = fopen(request_job_buffer->filename, "a");
					fd = fileno(fp);
					last_filename = request_job_buffer->filename;
				}
				
				#ifdef ELEVATED
				ksys_write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				#else
				write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				#endif
			}
			default: {
				break;
			}

		}
		
		request_job_buffer->status = REQUEST_COMPLETED;
		
		
	}
}

int server_write(job_buffer_t * job_buffer, char * floc, char * buf, size_t len){

	job_buffer->status = REQUEST_CREATED;
	job_buffer->cmd = CMD_WRITE;
	job_buffer->buf_len = len;
	strcpy(job_buffer->buf, buf);
	strcpy(job_buffer->filename, floc);
	//send request
	job_buffer->status = REQUEST_SENT;

	return 1;
}
