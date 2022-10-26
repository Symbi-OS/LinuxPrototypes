#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>


pthread_t tid[NUM_SLOTS];


int main(){

    workspace_t *workspace = server_init();

    if (workspace == (void *)-1){
        printf("Fail to intialize server...\n");
    }

    if (DEBUG) printf("Server intialized at: %p\n", workspace);

    for (int j = 0; j < NUM_SLOTS; j++){
        int err = pthread_create(&(tid[j]), NULL, &slot_thread, &workspace->slot[j]);
        if (err != 0) printf("can't create thread :[%s]", strerror(err));
    }

    for (int j = 0; j < NUM_SLOTS; j++){
        pthread_join(tid[j], NULL);
    }

    free(workspace);
    pthread_exit(NULL);

    return 0;
}