#include "common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>

#include <time.h>



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
job_buffer_t * get_job_buffer(){

	workspace_t * workspace = connect_server();

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

int server_write(job_buffer_t * job_buffer, char * buf, size_t len){
	//very stub write just doing the memory transfer

	job_buffer->cmd = CMD_WRITE;
	job_buffer->buf_len = len;
	strcpy(job_buffer->buf, buf);
	//strcpy(job_buffer->filename, floc);
	//send request
	job_buffer->status = JOB_REQUESTED;

	while (job_buffer->status != JOB_COMPLETED);
	return job_buffer->response;
}

int server_getppid(job_buffer_t * job_buffer){

	job_buffer->cmd = CMD_GETPPID;
	job_buffer->status = JOB_REQUESTED;

	while (job_buffer->status != JOB_COMPLETED);
	return job_buffer->response;
}
