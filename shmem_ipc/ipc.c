#include "ipc.h"
#include "common.h"

static const char* s_BackingFileName = "sym_server_shm";
static int s_BackingFileDescriptor = 0;
static void* s_SharedMemoryRegion = 0;

void* ipc_connect_client() {
	// Open the backing file
    s_BackingFileDescriptor = shm_open(s_BackingFileName, O_RDWR, S_IRUSR | S_IWUSR);
    if (s_BackingFileDescriptor < 0) {
        fprintf(stderr, "Failed to open the backing file\n");
        return NULL;
    }

    // Access the shared memory
    s_SharedMemoryRegion =
        mmap(NULL, SHMEM_REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s_BackingFileDescriptor, 0);


    if (s_SharedMemoryRegion == NULL) {
        fprintf(stderr, "Failed to mmap shared memory\n");
		close(s_BackingFileDescriptor);
        shm_unlink(s_BackingFileName);
        return NULL;
    }

	return s_SharedMemoryRegion;
}

void ipc_close() {
	if (s_SharedMemoryRegion != NULL) {
		munmap(s_SharedMemoryRegion, SHMEM_REGION_SIZE);
		close(s_BackingFileDescriptor);
		shm_unlink(s_BackingFileName);
	}
}

workspace_t* ipc_connect_server() {
	// Create the backing file
    s_BackingFileDescriptor = shm_open(
        s_BackingFileName,
        O_RDWR | O_CREAT,
        S_IRUSR | S_IWUSR
    );

    if (s_BackingFileDescriptor < 0) {
        fprintf(stderr, "Can't open shared memory segment\n");
        shm_unlink(s_BackingFileName);
        return NULL;
    }

    // Resize the backing file
    ftruncate(s_BackingFileDescriptor, SHMEM_REGION_SIZE);

    // Get access to the shared memory
    s_SharedMemoryRegion =
        mmap(NULL, SHMEM_REGION_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, s_BackingFileDescriptor, 0);

    if (s_SharedMemoryRegion == NULL) {
        fprintf(stderr, "Can't mmap the shared memory\n");
        shm_unlink(s_BackingFileName);
        return NULL;
    }

    // Zero out the backing file
    memset(s_SharedMemoryRegion, 0, SHMEM_REGION_SIZE);

    workspace_t* workspace = (workspace_t*)s_SharedMemoryRegion;
	return workspace;
}

/*
Function that for client to get empty job_buffer to communicate
*/
JobRequestBuffer_t* ipc_get_job_buffer(){

	workspace_t* workspace = (workspace_t*)ipc_connect_client();
    if (!workspace) {
        return NULL;
    }

	for (int i = 0; i < MAX_JOB_BUFFERS; i++){
		JobRequestBuffer_t* current = &(workspace->job_buffers[i]);
        //printf("Checking job buffer [%d] with status of %d\n", i, current->status);
		if (current->status == JOB_NO_REQUEST){
			// find a free spot!
			current->status = JOB_BUFFER_IN_USE;
            printf("[IPC Server] Connected to job buffer [%d]\n", i);
			return &workspace->job_buffers[i];
		}
	}

	return NULL;
}

void submit_job_request(JobRequestBuffer_t* jrb) {
    jrb->status = JOB_REQUESTED;
}

void mark_job_completed(JobRequestBuffer_t* jrb) {
    jrb->status = JOB_COMPLETED;
}

void wait_for_job_completion(JobRequestBuffer_t* jrb) {
    while (jrb->status != JOB_COMPLETED) {
        continue;
    }
}

void wait_for_job_request(JobRequestBuffer_t* jrb) {
    while (jrb->status != JOB_REQUESTED) {
        continue;
    }
}

void disconnect_job_buffer(JobRequestBuffer_t* jrb) {
    jrb->cmd = CMD_DISCONNECT;
    submit_job_request(jrb);
    wait_for_job_completion(jrb);
    memset(jrb, 0, sizeof(JobRequestBuffer_t));
}

void print_job_buffer(JobRequestBuffer_t* jrb){

    printf("JOB BUFFER:\n");
    printf("status: %d | fd: %d | pid: %d | cmd: %d \n", jrb->status, jrb->arg1, jrb->pid, jrb->cmd);
}