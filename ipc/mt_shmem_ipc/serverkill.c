#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>


void server_kill(workspace_t * workspace){
    
    for (int i = 0; i < NUM_JOB_BUFFERS; i++){
		job_buffer_t * current = &(workspace->job_buffers[i]);
		current->status = KILL_SIGNAL;
	}


    return;
}

int main(){

    workspace_t *workspace = connect_server();

    if (workspace == (void *)-1){
        printf("Fail to connect to server...\n");
        return -1;
    }


    server_kill(workspace);
    munmap(workspace, BACKING_FILE_SIZE);
    shm_unlink(BACKING_FILE);

    //clean up
}