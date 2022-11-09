
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// include for O_WRONLY, O_CREAT, O_TRUNC
#include <fcntl.h>
#include <time.h>

int main(int argc, char *argv[]) {
    // print argc

    // Check that we got 3 arguments total
    if (argc != 3) {
        printf("Got %d arguments, expected 3\n", argc);
        printf("Usage: ./write_loop <num bytes to write> <num times to write>\n");
        exit(1);
    }

    // First argument is number of bytes to write
    int bytes = atoi(argv[1]);
    // Second argument is number of times to write
    int times = atoi(argv[2]);

    // Allocate a buffer of the specified size
    char *buffer = malloc(bytes);
    // check that malloc worked
    if (buffer == NULL) {
        printf("malloc failed\n");
        exit(1);
    }

    // fill the buffer with the letter 'a'
    for (int i = 0; i < bytes; i++) {
        buffer[i] = 'a';
    }

    // open file /dev/null for writing
    int fd = open("/dev/null", O_WRONLY);

    // check open worked
    if (fd == -1) {
        printf("open failed\n");
        exit(1);
    }
    
    // Write bytes to /dev/null times times
    int rc;
    // Start timer
    time_t start,end;
    start=clock();//predefined  function in c

    for (int i = 0; i < times; i++) {
        rc = write(fd, buffer, bytes);
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
    printf("Throughput: %f Mb/s\n", (bytes*times)/(elapsed*1024*1024));
    printf("K Writes/sec: %f \n", (times)/(elapsed*1024));


    close(fd);
}
