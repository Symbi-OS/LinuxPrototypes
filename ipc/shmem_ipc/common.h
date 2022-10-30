#include <stdio.h>
#include <string.h>
#include <memory>
#include <unistd.h>
#include <stdlib.h>
#include <chrono>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

#define UNUSED(param) (void)param;

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct __attribute__((__packed__)) JobRequestBuffer {
    int cmd;           // Job command requested by the client
    int response;      // Response from the server
    uint8_t status;    // Flag indicating which stage the job is at
} JobRequestBuffer_t;

#define GET_NOW_TIME() std::chrono::high_resolution_clock::now()
#define START_CLOCK() auto _start_time = GET_NOW_TIME();
#define STOP_CLOCK() auto _end_time = GET_NOW_TIME();
#define GET_DURATION() std::chrono::duration<double>(_end_time - _start_time).count()

typedef int(*ksys_write_t)(unsigned int fd, const char* buf, size_t count);

