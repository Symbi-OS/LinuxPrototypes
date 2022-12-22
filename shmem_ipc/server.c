#include "common.h"
#include "ipc.h"
#include <sys/syscall.h>
#include <errno.h>
#include <pthread.h>

#define FD_PER_CLIENT 10000

int target_logfd = -1;
static uint8_t bShouldExit = 0;
pthread_spinlock_t locks[MAX_JOB_BUFFERS];

static int registered_fds[MAX_JOB_BUFFERS][FD_PER_CLIENT] = {};

struct thread_arg {
    int idx;
	int thread_count;
    workspace_t *ws;
};

void* workspace_thread(void* arg){
	struct thread_arg *data = (struct thread_arg *)arg;
	int start_idx = data->idx;
	int thread_count = data->thread_count;
	workspace_t *workspace = data->ws;
	register int idx = start_idx;
	JobRequestBuffer_t* job_buffer;

	while (!bShouldExit) {
		// obtain lock to update next buffer
        while (1) {
			if (workspace->job_buffers[idx].status == JOB_REQUESTED){
				goto JOB_FOUND;
			}else{
				idx += thread_count;
				if (idx >= MAX_JOB_BUFFERS){
					idx = start_idx;
				}
			}
        }
		
		JOB_FOUND:
		printf("Thread %d working on %d\n", start_idx, idx);
		job_buffer = &workspace->job_buffers[idx];
        // Process the requested command
		switch (job_buffer->cmd) {
		case CMD_WRITE: {
			int clientfd = job_buffer->arg1;
			if (registered_fds[idx][clientfd] == 0) {
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

				registered_fds[idx][clientfd] = borrowed_fd;
			}

			job_buffer->response = write(registered_fds[idx][clientfd], job_buffer->buffer, job_buffer->buffer_len);
			
			// Write to borrowed_fd
			if (job_buffer->response == -1) {
				perror("write failed");
			}

			break;
		}
		case CMD_CLOSE: {
			int clientfd = job_buffer->arg1;
			registered_fds[idx][clientfd] = 0;
			break;
		}
		case CMD_KILL_SERVER: {
			bShouldExit = 1;
			break;
		}
		case CMD_DISCONNECT: {
			memset(&registered_fds[idx], 0, sizeof(registered_fds[0]));
			break;
		}
		default: break;
		}

        // Updating the job status flag
		mark_job_completed(job_buffer);
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

	struct thread_arg args[num_threads];
	for (int j = 0; j < num_threads; j++){
		args[j].idx = j;
		args[j].thread_count = num_threads;
		args[j].ws = workspace;
		int err = pthread_create(&(tid[j]), NULL, &workspace_thread, &args[j]);
		printf("[IPC Server] Thread starting at: %p\n", &tid[j]);
		if (err != 0) printf("can't create thread :[%s]", strerror(err));
	}

	for (int j = 0; j < num_threads; j++){
		pthread_join(tid[j], NULL);
	}

	ipc_close();
	return 0;
}