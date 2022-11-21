#include "common.h"
#include "ipc.h"
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>

int target_logfd = -1;
int work_idx = 0;
pthread_mutex_t lock;

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
			//printf("Status: %d at Spinning at %d\n", workspace->job_buffers[work_idx].status, work_idx);
        	continue;
        }
		
		job_buffer = &workspace->job_buffers[idx];
		job_buffer->status = JOB_PICKEDUP;
		pthread_mutex_unlock(&lock);

		// Check if the server has been killed
		if (job_buffer->cmd == CMD_KILL_SERVER) {
			bShouldExit = 1;
			mark_job_completed(job_buffer);
			break;
		}


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

	int num_threads = 1;

	if (argc < 1){
		printf("Usage: ./<server binary> [optional]<nthreads>\n");
	} else if (argc == 2) {
		NUM_CLIENTS = atoi(argv[1]);
	}


	workspace_t* workspace = ipc_connect_server();


	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }


	
	pthread_t tid[num_threads];

	if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }

	for (int j = 0; j < num_threads; j++){
		int err = pthread_create(&(tid[j]), NULL, &workspace_thread, workspace);
		printf("Thread starting at: %p\n", &tid[j]);
		if (err != 0) printf("can't create thread :[%s]", strerror(err));
	}

	for (int j = 0; j < num_threads; j++){
		pthread_join(tid[j], NULL);
	}

	ipc_close();
	return 0;
}