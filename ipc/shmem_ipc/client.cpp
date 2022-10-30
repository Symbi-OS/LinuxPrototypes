#include "common.h"
extern "C" {
#include <LINF/sym_all.h>
}

constexpr const char* BackingFileName = "sym_server_shm";
constexpr int BackingFileSize = 512;

#ifdef INDEPENDENT_CLIENT
void start_independent_client_loop(int iterations) {
	auto log = std::unique_ptr<FILE, decltype(&fclose)>(fopen("run_log", "w"), &fclose);
	int logfd = fileno(log.get());

#ifdef ELEVATED
	ksys_write_t ksys_write = (ksys_write_t)sym_get_fn_address((char*)"ksys_write");
	sym_elevate();
#endif

	START_CLOCK();
	for (int i = 0; i < iterations; ++i) {
#ifdef ELEVATED
        if (i % 20 == 0) {
			write(logfd, "ksys_write\r", 11);
		} else {
			ksys_write(logfd, "ksys_write\r", 11);
		}
#else
		write(logfd, "ksys_write\r", 11);
#endif
	}
	STOP_CLOCK();

	auto time_used = GET_DURATION();
	printf("Time used: %f\n", time_used);

#ifdef ELEVATED
	sym_lower();
#endif
}
#else
void start_shared_memory_client_loop(int iterations) {
	// Open the backing file
	int fd = shm_open(BackingFileName, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		fprintf(stderr, "Failed to open the backing file\n");
		return;
	}

	// Access the shared memory
	void* shm = mmap(NULL, BackingFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	// Start the stress-testing loop
	volatile auto job_buffer = static_cast<JobRequestBuffer_t*>(shm);
	job_buffer->cmd = 1;

	START_CLOCK();
	for (int i = 0; i < iterations; ++i) {
		// Indicate that the job was requested
		job_buffer->status = JOB_REQUESTED;

		while (job_buffer->status != JOB_COMPLETED) {
			continue;
		}
	}
	STOP_CLOCK();

	auto time_used = GET_DURATION();
    printf("Time used: %f\n", time_used);

	// Cleanup
	munmap(shm, BackingFileSize);
	close(fd);
}
#endif

int main(int argc, char** argv) {
	UNUSED(argc);

	int iterations = atoi(argv[1]);

#ifdef INDEPENDENT_CLIENT	
	start_independent_client_loop(iterations);
#else
	start_shared_memory_client_loop(iterations);
#endif
	return 0;
}

