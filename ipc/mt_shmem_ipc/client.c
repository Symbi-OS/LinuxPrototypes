#include "shmem.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define WRITE_BUF "BlackMagic\n"
#define WRITE_LOC "tmp.txt"

#define DEBUG 0

void write_task_independent(int iteration){
    FILE * fp;
    int fd;

    fp = fopen(WRITE_LOC, "a");
    fd = fileno(fp);

    for (int i = 0; i < iteration; i++){
        volatile int j;
        for (j = 0; j < 10000; j++); 
        write(fd, WRITE_BUF, strlen(WRITE_BUF));
    }

    close(fd);
    return;
}

void write_task(int iteration, job_buffer_t *work_job_buffer){

    for (int i = 0; i < iteration; i++){
        volatile int j;
        for (j = 0; j < 10000; j++); 
        
        server_write(work_job_buffer, WRITE_LOC, WRITE_BUF, strlen(WRITE_BUF));
        
        if (DEBUG) printf("request %ld byte to write in %s\n", work_job_buffer->buf_len, work_job_buffer->filename);
        // Wait for the job to be completed
        while (work_job_buffer->status != REQUEST_COMPLETED ) {
            continue;
        }

        if (DEBUG) printf("recevied response from server\n");
    }

    //complete work
    if (work_job_buffer != NULL){
        work_job_buffer->status = EMPTY_JOB_BUFFER;
    }

    return;
}


int main(int argc, char** argv){

    int iterations;
    int if_connect;
    clock_t start, end;
    double cpu_time_used;

    if(argc == 3){
        iterations = atoi(argv[1]);
        if_connect = atoi(argv[2]);
    }else{
        iterations = 50;
        if_connect = 0;
    }

    if (!if_connect){
        start = clock();
        write_task_independent(iterations);
        end = clock();
        cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  	    printf("%f", cpu_time_used);
        return 0;
    }

    workspace_t *workspace = connect_server();

    if (workspace == (void *)-1){
        printf("Fail to connect to server...\n");
        return -1;
    }

    job_buffer_t * work_job_buffer = get_job_buffer(workspace);
    if (work_job_buffer == (void *)-1) {
        printf("Fail to request working job_buffer...\n");
        return 1;
    }

    if (DEBUG) printf("connected to job_buffer at %p\n", work_job_buffer);

    start = clock();
    write_task(iterations, work_job_buffer);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("%f", cpu_time_used);

    munmap(workspace, BACKING_FILE_SIZE);
    return 0;
}
