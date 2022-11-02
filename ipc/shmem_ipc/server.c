#include "common.h"
#include <LINF/sym_all.h>
#include <pthread.h>


int main(int argc, char** argv) {
	UNUSED(argc);

	if (argc < 2){
		printf("Usage: ./<server binary> <num threads>\n");
		printf("currently max thread is 2\n");
	}

	int num_threads = atoi(argv[1]);

	workspace_t *workspace = server_init();

	printf("server started at %p\n", workspace);

    if (workspace == (void *)-1){
        printf("Fail to intialize server...\n");
    }

    if (num_threads == 1){
		//if it is a single thread, then just start the process
        job_buffer_thread(&workspace->job_buffers[0]);
    }else{
		//use pthread to organize all threads
		pthread_t tid[num_threads];
		for (int j = 0; j < num_threads; j++){
			int err = pthread_create(&(tid[j]), NULL, &job_buffer_thread, &workspace->job_buffers[j]);
			if (err != 0) printf("can't create thread :[%s]", strerror(err));
		}

		for (int j = 0; j < num_threads; j++){
			pthread_join(tid[j], NULL);
		}
	}

    //close up the server
    munmap(workspace, BACKING_FILE_SIZE);
    shm_unlink(BACKING_FILE);

	return 0;
}
