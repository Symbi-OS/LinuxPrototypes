#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

/*
Function that initalize the server and workspace
*/
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

	memset(shared_memory_ptr, 0x0, BACKING_FILE_SIZE);

	// points to the next available spot

	workspace_t * workspace = (workspace_t *)shared_memory_ptr;

	return workspace;
}


/*
Function that for client to get empty job_buffer to communicate
*/
job_buffer_t * get_job_buffer(workspace_t * workspace){

	for (int i = 0; i < NUM_JOB_BUFFERS; i++){
		if (workspace->job_buffers[i].status == EMPTY_JOB_BUFFER){
			//find a free spot!
			workspace->job_buffers[i].status = JOB_BUFFER_IN_USE;
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

	while (1){ // ever running thread


		//wait for request
		while (request_job_buffer->status != REQUEST_SENT) {
			continue;
		}

		request_job_buffer->status = REQUEST_RECEIVED;

		request_job_buffer->status = REQUEST_PROCESSING;

		switch(request_job_buffer->cmd){
			case 0x1: {

				if ((fd == -1) | strcmp(request_job_buffer->filename, last_filename)){
					fp = fopen(request_job_buffer->filename, "a");
					fd = fileno(fp);
					last_filename = request_job_buffer->filename;
				}
				
				ssize_t result = write(fd, request_job_buffer->buf, request_job_buffer->buf_len);
				if (result != (ssize_t)request_job_buffer->buf_len){
					printf("Write failed, only write %ld\n", result);
				}
				break;
			}
			default: {
				break;
			}

		}
		
		request_job_buffer->status = REQUEST_COMPLETED;
		
		
	}

}