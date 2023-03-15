#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

// #define DEBUG 1
struct params {
    int bytes;
    int times;
    char *path;
};

void parse_args(int argc, char *argv[], struct params *p) {
    // Check that we got 3 arguments total
    if (argc != 4) {
        printf("Got %d arguments, expected 4\n", argc);
        printf("Usage: ./write_loop <num bytes to write> <num times to write> <file path>\n");
        exit(1);
    }

    p->bytes = atoi(argv[1]);
    p->times = atoi(argv[2]);
    p->path = argv[3];

}

char *get_buf(struct params *p) {
    // Allocate a buffer of the specified size
    char *buffer = (char *) malloc(p->bytes);
    if (buffer == NULL) {
        printf("malloc failed\n");
        exit(1);
    }

    // fill the buffer with the letter 'a'
    for (int i = 0; i < p->bytes; i++) {
        buffer[i] = 'a';
    }

    return buffer;
}

int prepare_file(struct params *p){

    // Skip deletion if it's /dev/null
    if (strcmp(p->path, "/dev/null") == 0) {
        printf("Skipping deletion of /dev/null\n");
    } else {
        if (access(p->path, F_OK) != -1) {
            printf("File %s exists, deleting\n", p->path);
            int rc = remove(p->path);
            // check that remove worked
            if (rc != 0) {
                printf("remove failed\n");
                exit(1);
            }
        }
    }

    int fd = open(p->path, O_WRONLY | O_CREAT);
    if (fd == -1) {
        printf("open failed\n");
        exit(1);
    }
    return fd;
}
#ifdef LATENCY
void process_latencies(struct timespec *times, int count){
    // If you input count is n, there are n+1 times and n latencies
    // fprintf(stderr, "Processing latencies count is %d\n", count);

    // Calculate the individual latencies
    // fprintf(stderr, "calloc latencies, count is %d, size of timespec is %d\n", count, sizeof(struct timespec));
    struct timespec *latencies = calloc(count, sizeof(struct timespec));
    // check for error
    if (latencies == NULL) {
        printf("calloc failed\n");
        exit(1);
    }

    for (int i = 0; i < count; i++) {
        // fprintf(stderr, "times[%d] s:ns %ld:%ld\n", i, times[i].tv_sec, times[i].tv_nsec);
        // fprintf(stderr, "times[%d] s:ns %ld:%ld\n", i+1, times[i+1].tv_sec, times[i+1].tv_nsec);

        struct timespec diff;
        // seconds
        diff.tv_sec = times[i+1].tv_sec - times[i].tv_sec;
        // ns
        diff.tv_nsec = times[i+1].tv_nsec - times[i].tv_nsec;
        // fprintf(stderr, "diff.tv_sec %ld diff.tv_nsec %ld\n", diff.tv_sec, diff.tv_nsec);
        assert(diff.tv_sec == 0 || diff.tv_sec == 1);

        if (diff.tv_sec == 1) {
            diff.tv_nsec += 1000 * 1000 *1000;
        }
        // fprintf(stderr, "diff.tv_sec %ld diff.tv_nsec %ld\n", diff.tv_sec, diff.tv_nsec);

        // Latencies are in nanoseconds, so calc diff

        latencies[i].tv_nsec = diff.tv_nsec;
        // fprintf(stderr, "~~~latencies[%d] s:ns %ld:%ld\n", i, latencies[i].tv_sec, latencies[i].tv_nsec);
        // fprintf(stderr, "latency[%d] is %ld\n", i, latencies[i].tv_nsec);
    }
    // if file exists, remove it
    if (access("latencies.txt", F_OK) != -1) {
        // printf("File latencies.txt exists, deleting\n");
        int rc = remove("latencies.txt");
        // check that remove worked
        if (rc != 0) {
            printf("remove failed\n");
            exit(1);
        }
    }
    // fprintf(stderr, "opening file\n");
    FILE *fp = fopen("latencies.txt", "w");
    // check for error
    if (fp == NULL) {
        printf("fopen failed\n");
        exit(1);
    }
    // fprintf(stderr, "count is %d, entering print loop\n", count);
    for (int i = 0; i < count; i++) {
        // fprintf(stderr, "got here\n");
        // fprintf(stderr, "latency[%d] is %ld\n", i, latencies[i].tv_nsec);
        fprintf(fp, "%ld\n", latencies[i].tv_nsec);
    } 
    fclose(fp);
    free(latencies);
}
#endif

void benchmark(struct params *p){
    char *buffer = get_buf(p);

    int fd = prepare_file(p);
    time_t start, end;
#if DEBUG
    time_t prior, current;
    int print_interval = 1<<20;
#endif
    start=clock();


    // get prior time
#ifdef DEBUG
    prior = clock();
#endif

#ifdef LATENCY
    // If the user inputs count is n, there are n+1 times.
    struct timespec *times = calloc(p->times+1, sizeof(struct timespec));
    clock_gettime(CLOCK_MONOTONIC_RAW, &times[0]); 
#endif

    for (int i = 0; i < p->times; i++) {

#ifdef DEBUG
        // This does not seem to pertubate the timing
        if( (i % print_interval) == (print_interval - 1) ){
            // get current time
            current = clock();
            // print time since last print
            printf("Time: %f seconds\n", (double)(current-prior)/CLOCKS_PER_SEC);
            prior = current;
        }
#endif

        int rc = write(fd, buffer, p->bytes);
        // check write worked
        if (rc == -1) {
            printf("write failed\n");
            exit(1);
        }
#ifdef LATENCY
        clock_gettime(CLOCK_MONOTONIC_RAW, &times[i+1]); 
#endif
    }

    end=clock();

    // Subtract start from end to get elapsed time
    double elapsed = (double)(end-start)/CLOCKS_PER_SEC;
    // Print elapsed time
    printf("Elapsed time: %f\n", elapsed);
    // Print throughput
    printf("Throughput: %f Mb/s\n", (p->bytes*p->times)/(elapsed*1024*1024));
    printf("K Writes/sec: %f \n", (p->times)/(elapsed*1024));

#ifdef LATENCY
    process_latencies(times, p->times);
    free(times);
#endif

    free(buffer);
    close(fd);
}

int main(int argc, char *argv[]) {
    struct params p;

    parse_args(argc, argv, &p); 

    benchmark(&p);
}
