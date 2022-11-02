#include "common.h"

void server_kill(workspace_t * workspace){
    
    for (int i = 0; i < MAX_JOB_BUFFERS; i++){
		job_buffer_t * current = &(workspace->job_buffers[i]);
		current->status = SERVER_KILL_SIGNAL;
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