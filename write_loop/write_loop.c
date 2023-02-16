
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

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
    char *buffer = malloc(p->bytes);
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
void benchmark(struct params *p){
    char *buffer = get_buf(p);

    int fd = prepare_file(p);
    time_t start, end, prior, current;
    start=clock();

    int print_interval = 1<<20;

    // get prior time
    prior = clock();
    int verbose = 1;

    // Start at 1 to make timer check easier, since we don't want to count the first iteration 
    for (int i = 1; i < p->times; i++) {

        // This does not seem to pertubate the timing
        if( verbose && ( i % print_interval == 0 ) ){
            // get current time
            current = clock();
            // print time since last print
            printf("Time: %f seconds\n", (double)(current-prior)/CLOCKS_PER_SEC);
            prior = current;
        }

        int rc = write(fd, buffer, p->bytes);
        // check write worked
        if (rc == -1) {
            printf("write failed\n");
            exit(1);
        }
    }

    end=clock();

    // Subtract start from end to get elapsed time
    double elapsed = (double)(end-start)/CLOCKS_PER_SEC;
    // Print elapsed time
    printf("Elapsed time: %f\n", elapsed);
    // Print throughput
    printf("Throughput: %f Mb/s\n", (p->bytes*p->times)/(elapsed*1024*1024));
    printf("K Writes/sec: %f \n", (p->times)/(elapsed*1024));

    free(buffer);
    close(fd);
}

int main(int argc, char *argv[]) {
    struct params p;

    parse_args(argc, argv, &p); 

    benchmark(&p);
}
