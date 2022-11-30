#include "common.h"
#include "ipc.h"
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>
#define _POSIX_SOURCE
#include <signal.h>
#include <stdio.h>
#include <unistd.h>


int target_logfd = -1;
static uint8_t bShouldExit = 0;
pthread_spinlock_t locks[MAX_JOB_BUFFERS];


static int client_fds[MAX_JOB_BUFFERS] = { 0 };
static int server_fds[MAX_JOB_BUFFERS] = { 0 };

void* workspace_thread(void* ws){
	workspace_t *workspace = (workspace_t *) ws;
	register int idx = 0;
	JobRequestBuffer_t* job_buffer;

	while (!bShouldExit) {
		// obtain lock to update next buffer
        while (1) {
			if (pthread_spin_trylock(&locks[idx]) == 0){
				if (workspace->job_buffers[idx].status == JOB_REQUESTED){
					goto JOB_FOUND;
				}else{
					pthread_spin_unlock(&locks[idx]);
					idx++;
					if (idx==MAX_JOB_BUFFERS){
						idx = 0;
					}
				}
			}else{
				idx++;
				if (idx==MAX_JOB_BUFFERS){
					idx = 0;
				}
			}
        }
		
		JOB_FOUND:
		job_buffer = &workspace->job_buffers[idx];
        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_WRITE: {
			if (client_fds[idx] != job_buffer->arg1) {
				printf("new fd: %d\n", job_buffer->arg1);
				int pidfd = syscall(SYS_pidfd_open, job_buffer->pid, 0);
				int borrowed_fd = syscall(SYS_pidfd_getfd, pidfd, job_buffer->arg1, 0);
				// check error case
				if (borrowed_fd == -1) {
					perror("pidfd_getfd");
					//  print errno
					printf("errno: %d\n", errno);
					//  print strerror
					printf("strerror: %s\n", strerror(errno));
					mark_job_completed(job_buffer);
					break;
				}
				server_fds[idx] = borrowed_fd;
			}

			job_buffer->response = write(server_fds[idx], job_buffer->buffer, job_buffer->buffer_len);

			// Write to borrowed_fd
			if (job_buffer->response == -1) {
				perror("write failed");
			}
			// Updating the job status flag
			mark_job_completed(job_buffer);
			break;
		}
		case CMD_KILL_SERVER: {
			bShouldExit = 1;
			// Updating the job status flag
			mark_job_completed(job_buffer);
			break;
		}
		case CMD_DISCONNECT:{
			memset(job_buffer, 0, sizeof(JobRequestBuffer_t));
			break;
		}
		default: break;
		}

		pthread_spin_unlock(&locks[idx]);
	}

	pthread_exit(NULL);
}



int main(int argc, char** argv) {

	int num_threads = 1;

	if (argc < 1){
		printf("Usage: ./<server binary> [optional]<nthreads>\n");
	} else if (argc == 2) {
		num_threads = atoi(argv[1]);
	}


	workspace_t* workspace = ipc_connect_server();


	if (workspace == NULL){
        printf("Fail to intialize server...\n");
    }


	
	pthread_t tid[num_threads];

	for (int j = 0; j < MAX_JOB_BUFFERS; j++){
		if (pthread_spin_init(&(locks[j]), PTHREAD_PROCESS_PRIVATE) != 0) {
			printf("\n mutex init has failed\n");
			return 1;
		}
	}

	for (int j = 0; j < num_threads; j++){
		int err = pthread_create(&(tid[j]), NULL, &workspace_thread, workspace);
		printf("[IPC server] Thread starting at: %p\n", &tid[j]);
		if (err != 0) printf("can't create thread :[%s]", strerror(err));
	}

	for (int j = 0; j < num_threads; j++){
		pthread_join(tid[j], NULL);
	}

	ipc_close();
	return 0;
}
