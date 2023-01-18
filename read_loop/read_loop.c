#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
// include for O_WRONLY, O_CREAT, O_TRUNC
#include <fcntl.h>
#include <time.h>
// include for errno
#include <errno.h>
// include for strerror
#include <string.h>

#include <syscall.h>

int main(int argc, char *argv[]) {
  printf("Starting read loop main\n");
  // print arg

  // Check that we got 3 arguments total
  if (argc != 3) {
    printf("Got %d arguments, expected 3\n", argc);
    printf("Usage: ./rd_loop <num bytes to read> <num times to read>\n");
    exit(1);
  }

  // First argument is number of bytes to read
  int bytes = atoi(argv[1]);
  // Second argument is number of times to read
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

  // open /dev/null for reading
  // int fd = open("/dev/null", O_RDONLY);
  // int fd = open("/dev/random", O_RDONLY);
  int fd = open("./test.txt", O_RDONLY);

  // check open worked
  if (fd == -1) {
    printf("open failed\n");
    // print errno string
    printf("errno: %s\n", strerror(errno));
    exit(1);
  }

  // read /dev/null times times
  int rc;
  // Start timer
  time_t start, end;
  start = clock(); // predefined  function in c

  for (int i = 0; i < times; i++) {
    // rc = syscall(SYS_pread64, fd, buffer, bytes, 0);
    rc = syscall(SYS_read, fd, buffer, bytes);
    if (rc == -1) {
      printf("read failed\n");
      // print errno
      printf("errno: %d\n", errno);
      // print errno string
      printf("errno: %s\n", strerror(errno));
      exit(1);
    }
  }

  end = clock();
  // Subtract start from end to get elapsed time
  double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
  // Print elapsed time
  printf("Elapsed time: %f\n", elapsed);
  // Print throughput
  printf("Throughput: %f Mb/s\n", (bytes * times) / (elapsed * 1024 * 1024));
  printf("K reads/sec: %f \n", (times) / (elapsed * 1024));

  close(fd);
}
