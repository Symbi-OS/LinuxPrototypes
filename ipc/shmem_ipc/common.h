#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "LINF/sym_all.h"

#define UNUSED_PARAM(param) (void)param;

#define JOB_NO_REQUEST 0
#define JOB_REQUESTED  1
#define JOB_COMPLETED  2

typedef struct JobRequestBuffer {
    int fd;            // File descriptor
    int cmd;           // Job command requested by the client
    int response;      // Response from the server
    int status;        // Flag indicating which stage the job is at
} JobRequestBuffer_t;

#ifdef ELEVATED_MODE
typedef int(*ksys_write_t)(unsigned int fd, const char *buf, size_t count);
#endif

