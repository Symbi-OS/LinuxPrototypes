#include "common.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "LINF/sym_all.h"

static const char* BackingFileName = "sym_server_shm";
static const int BackingFileSize = 512;

#ifdef ELEVATED_MODE
static ksys_write_t my_ksys_write = NULL;
#endif

int main(int argc, char** argv) {
	UNUSED_PARAM(argc)

	// Get the number of stress test iterations from the command line
	int iteration_count = atoi(argv[1]);

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

#ifdef ELEVATED_MODE
	// Get the adress of ksys_write
	my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");
	fprintf(stderr, "ksys_write: %p\n", my_ksys_write);
#endif

	// Create a file to write to
	FILE* log = fopen("stress_test_shared_memory_log", "w");
	int logfd = fileno(log);
	
	// Prepare the job buffer
	volatile JobRequestBuffer_t* job_buffer = (JobRequestBuffer_t*)shared_memory;
	int resp = 1;

#ifdef ELEVATED_MODE
	sym_elevate();
#endif

	// Begin stress testing
	for (int i = 0; i < iteration_count; ++i) {
		// Wait until we get the job
        	while (job_buffer->status != JOB_REQUESTED) {
			continue;
       		}

        // Process the requested command
		switch (job_buffer->cmd) {
		case 1: {
#ifdef ELEVATED_MODE
			if (i % 20 == 0) {
				write(logfd, "ksys_write\r", 11);
			} else {
				my_ksys_write(logfd, "ksys_write\r", 11);
			}
#else
				write(logfd, "ksys_write\r", 11);
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

#ifdef ELEVATED_MODE
	sym_lower();
#endif

	// Close the log file
	fclose(log);

	// Cleanup
	munmap(shared_memory, BackingFileSize);
	close(fd);
	shm_unlink(BackingFileName);

	return 0;
}

