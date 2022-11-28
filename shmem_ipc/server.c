#include "common.h"
#include "ipc.h"
#include <sys/syscall.h>
#include <errno.h>

int ITERATIONS;
int target_logfd = -1;
int NUM_CLIENTS = 1;

static uint8_t bShouldExit = 0;

static int registered_fds[10000] = { 0 };

void workspace_thread(workspace_t* workspace) {
	// Begin stress testing (+2 is necessary for open and close calls)
	register int idx = 0;
	while (!bShouldExit) {
		// Wait until we get the job
        while (workspace->job_buffers[idx].status != JOB_REQUESTED) {
			idx++;
			if (idx==NUM_CLIENTS){
				idx = 0;
			}
        	continue;
        }

		JobRequestBuffer_t* job_buffer = &workspace->job_buffers[idx];

		// Check if the server has been killed
		if (job_buffer->cmd == CMD_KILL_SERVER) {
			bShouldExit = 1;
			mark_job_completed(job_buffer);
			break;
		}

        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_WRITE: {
			int clientfd = job_buffer->arg1;
			if (registered_fds[clientfd] == 0) {
				int pidfd = syscall(SYS_pidfd_open, job_buffer->pid, 0);
				int borrowed_fd = syscall(SYS_pidfd_getfd, pidfd, job_buffer->arg1, 0);

				// check error case
				if (borrowed_fd == -1) {
					perror("pidfd_getfd");
					//  print errno
					printf("errno: %d\n", errno);
					//  print strerror
					printf("strerror: %s\n", strerror(errno));
					break;
				}

				registered_fds[clientfd] = borrowed_fd;
			}

			job_buffer->response = write(registered_fds[clientfd], job_buffer->buffer, job_buffer->buffer_len);

			// Write to borrowed_fd
			if (job_buffer->response == -1) {
				perror("write failed");
			}
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
	}
}

int main(int argc, char** argv) {
	if (argc < 1){
		printf("Usage: ./<server binary> [optional]<nthreads>\n");
	} else if (argc == 2) {
		NUM_CLIENTS = atoi(argv[1]);
	}

	workspace_t* workspace = ipc_connect_server();

	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }

	workspace_thread(workspace);

	ipc_close();
	return 0;
}