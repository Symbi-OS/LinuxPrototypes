#include "common.h"
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

	for (int i = 0; i < MAX_JOB_BUFFERS; i++){
		job_buffer_t * current = &(workspace->job_buffers[i]);
		if (current->status == JOB_NO_REQUEST){
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

	#ifdef ELEVATED
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	getppid_t getppid_elevated = (getppid_t)sym_get_fn_address((char*)"__x64_sys_getppid");
	sym_elevate();
	#endif

	printf("thread started at %p\n", job_buffer);
	while (1){ // ever running thread
		//wait for request
		while (request_job_buffer->status != JOB_REQUESTED) {
			if (request_job_buffer->status == SERVER_KILL_SIGNAL){
				#ifdef ELEVATED
				sym_lower();
				#endif
				pthread_exit (NULL);
			}
			continue;
		}

		switch(request_job_buffer->cmd){

			case CMD_WRITE: {
				
				break;
			}

			case CMD_GETPPID: {
				#ifdef ELEVATED
				getppid_elevated();
				#else
				getppid();
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
}

int server_write(job_buffer_t * job_buffer, char * floc, char * buf, size_t len){

	job_buffer->cmd = CMD_WRITE;
	job_buffer->status = JOB_REQUESTED;
	strcpy(job_buffer->buf, floc);
	strcat(job_buffer->buf, "|");
	strcat(job_buffer->buf, buf);
	char * len_str = "0000";
	strcat(job_buffer->buf, "|");
	sprintf(len_str, "%lu", len);
	strcat(job_buffer->buf, len_str);

	while (job_buffer->status != JOB_COMPLETED);
	return job_buffer->response;
}

int server_getppid(job_buffer_t * job_buffer){

	job_buffer->cmd = CMD_GETPPID;
	job_buffer->status = JOB_REQUESTED;

	while (job_buffer->status != JOB_COMPLETED);
	return job_buffer->response;
}

