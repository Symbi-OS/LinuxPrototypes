#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define UNUSED(param) (void)param;

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct __attribute__((__packed__)) JobRequestBuffer {
    int cmd;           // Job command requested by the client
    int response;      // Response from the server
    int status;    // Flag indicating which stage the job is at
} JobRequestBuffer_t;

#define START_CLOCK() clock_t _start_time = clock();
#define STOP_CLOCK() clock_t _end_time = clock();
#define GET_DURATION() ((double)(_end_time - _start_time)) / CLOCKS_PER_SEC;

typedef int(*ksys_write_t)(unsigned int fd, const char* buf, size_t count);

