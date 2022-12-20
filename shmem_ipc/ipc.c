#include "ipc.h"
#include "common.h"

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

static const char* s_BackingFileName = "sym_server_shm";
static int s_BackingFileDescriptor = 0;
static void* s_SharedMemoryRegion = 0;


int futex(int *uaddr, int futex_op, int val, const struct timespec *timeout, int *uaddr2, int val3) {
    return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr2, val3);
}

/* Acquire the futex pointed to by 'futexp': wait for its value to
    become 1, and then set the value to 0. */
void futex_wait(int *futexp) {
    int s;

    while (1) {

        /* Is the futex available? */

        if (__sync_bool_compare_and_swap(futexp, 1, 0))
            break;      /* Yes */

        /* Futex is not available; wait */

        s = futex(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (s == -1 && errno != EAGAIN)
            handle_error("futex-FUTEX_WAIT");
    }
}

/* Release the futex pointed to by 'futexp': if the futex currently
    has the value 0, set its value to 1 and the wake any futex waiters,
    so that if the peer is blocked in fpost(), it can proceed. */

void futex_signal(int *futexp) {
    int s;

    /* __sync_bool_compare_and_swap() was described in comments above */

    if (__sync_bool_compare_and_swap(futexp, 0, 1)) {

        s = futex(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s  == -1)
            handle_error("futex-FUTEX_WAKE");
    }
}


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
    futex_signal(&(jrb->lock));
}

void wait_for_job_completion(JobRequestBuffer_t* jrb) {
    futex_wait(&(jrb->lock));
    if (jrb->status != JOB_COMPLETED){
        handle_error(" lock is released before a job completes");
    }
}

void wait_for_job_request(JobRequestBuffer_t* jrb) {
    if (jrb->status != JOB_REQUESTED) {
        handle_error(" lock is released before a job completes");
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