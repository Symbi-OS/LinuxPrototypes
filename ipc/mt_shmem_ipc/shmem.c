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
Function that for client to get empty slot to communicate
*/
slot_t * get_slot(workspace_t * workspace){

	for (int i = 0; i < NUM_SLOTS; i++){
		if (workspace->slot[i].status == EMPTY_SLOT){
			//find a free spot!
			workspace->slot[i].status = SLOT_IN_USE;
			return &workspace->slot[i];
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

void* slot_thread(void *slot){
	
	slot_t *request_slot = slot;

	FILE * fp = NULL;
	int fd = -1;
	char * last_filename = "";

	if (DEBUG) printf("Thread at slot %p is up\n", request_slot);

	while (1){ // ever running thread


		//wait for request
		while (request_slot->status != REQUEST_SENT) {
			continue;
		}

		request_slot->status = REQUEST_RECEIVED;
		if (DEBUG) printf("Thread at slot %p recevied a request\n", request_slot);

		request_slot->status = REQUEST_PROCESSING;
		switch(request_slot->cmd){
			case CMD_WRITE:{

				if ((fd == -1) | strcmp(request_slot->filename, last_filename)){
					fp = fopen(request_slot->filename, "a");
					fd = fileno(fp);
					last_filename = request_slot->filename;
				}
				
				if (DEBUG) printf("Writing %ld byte to %s\n", request_slot->buf_len, request_slot->filename);
				ssize_t result = write(fd, request_slot->buf, request_slot->buf_len);
				if (result != (ssize_t)request_slot->buf_len){
					printf("Write failed, only write %ld\n", result);
				}
				break;
			}
			default:{
				break;
			}

		}

		if (DEBUG) printf("Thread at slot %p completed a request\n", request_slot);
		request_slot->status = REQUEST_COMPLETED;

		
	}

}