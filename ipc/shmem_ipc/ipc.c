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

void ipc_close_client() {
	if (s_SharedMemoryRegion != NULL) {
		munmap(s_SharedMemoryRegion, SHMEM_REGION_SIZE);
		close(s_BackingFileDescriptor);
		shm_unlink(s_BackingFileName);
	}
}

