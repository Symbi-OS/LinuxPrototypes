#include "common.h"
#include <LINF/sym_all.h>

static const char* BackingFileName = "sym_server_shm";
static int BackingFileSize = 512;

int main(int argc, char** argv) {
	UNUSED(argc);

	// Get the number of stress test iterations from the command line
	int iterations = atoi(argv[1]);

	// Create the backing file
	int fd = shm_open(
		BackingFileName,
		O_RDWR | O_CREAT,
		S_IRUSR | S_IWUSR
	);

	if (fd < 0) {
		printf("Can't open shared memory segment\n");
		shm_unlink(BackingFileName);
		return -1;
	}

	// Resize the backing file
	ftruncate(fd, BackingFileSize);

	// Get access to the shared memory
	void* shared_memory =
		mmap(NULL, BackingFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (shared_memory == (void*)-1) {
		printf("Can't mmap the shared memory\n");
		shm_unlink(BackingFileName);
		return -1;
	}

	// Zero out the backing file
	memset(shared_memory, 0, BackingFileSize);

#ifdef ELEVATED
	// Get the adress of ksys_write
	//ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address((char*)"ksys_write");
	//fprintf(stderr, "ksys_write: %p\n", ksys_write);

	getppid_t getppid_elevated = (getppid_t)sym_get_fn_address((char*)"__x64_sys_getppid");
	fprintf(stderr, "getppid: %p\n", getppid);

	sym_elevate();
#endif

	// Create a file to write to
    //int logfd = open("run_log", O_CREAT | O_WRONLY, S_IRUSR);
	
	// Prepare the job buffer
	volatile JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 1;

	// Begin stress testing
	for (int i = 0; i < iterations; ++i) {
		// Wait until we get the job
        while (job_buffer->status != JOB_REQUESTED) {
			continue;
       	}

        // Process the requested command
		switch (job_buffer->cmd) {
		case 1: {
#ifdef ELEVATED
			/*
			if (i % 20 == 0) {
				write(logfd, "ksys_write\r", 11);
			} else {
				ksys_write(logfd, "ksys_write\r", 11);
			}*/
			getppid_elevated();
#else
			//write(logfd, "ksys_write\r", 11);
			getppid();
#endif
			break;
		}
		default: break;
		}

		// Writing in a response
		job_buffer->response = resp;
		++resp;

        // Updating the job status flag
		job_buffer->status = JOB_COMPLETED;
	}

#ifdef ELEVATED
	sym_lower();
#endif

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);
	//close(logfd);
	shm_unlink(BackingFileName);

	return 0;
}
