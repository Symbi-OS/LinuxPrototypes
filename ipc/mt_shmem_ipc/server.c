#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define DEBUG 1

pthread_t tid[NUM_JOB_BUFFERS];


int main(){


    workspace_t *workspace = server_init();
    if (workspace == (void *)-1){
        printf("Fail to intialize server...\n");
    }

    if (DEBUG){
        printf("Server intialized at: %p\n", workspace);
        job_buffer_thread(&workspace->job_buffers[0]);
    }

    for (int j = 0; j < NUM_JOB_BUFFERS; j++){
        int err = pthread_create(&(tid[j]), NULL, &job_buffer_thread, &workspace->job_buffers[j]);
        if (err != 0) printf("can't create thread :[%s]", strerror(err));
    }

    for (int j = 0; j < NUM_JOB_BUFFERS; j++){
        pthread_join(tid[j], NULL);
    }

    free(workspace);
    pthread_exit(NULL);

    return 0;
}