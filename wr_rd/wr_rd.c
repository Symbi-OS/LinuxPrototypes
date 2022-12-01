#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// A function that reads from a file descriptor and prints the result
void read_me() {
  char buf[100];
  int fd = open("test.txt", O_RDWR, 0666);
  if (fd < 0) {
    printf("Error opening file\n");
    exit(1);
  }

  int n = read(fd, buf, 100);
  printf("Read %d bytes: %s\n", n, buf);
  close(fd);
}

// A function that writes to a file descriptor.
void write_me() {

  int fd = open("test.txt", O_RDWR | O_CREAT, 0666);
  if (fd < 0) {
    printf("Error opening file\n");
    exit(1);
  }

  char *buf = "Hello World";
  int n = write(fd, buf, 12);
  printf("Wrote %d bytes\n", n);

  close(fd);
}

int main(int argc, char *argv[]) {
  // Open a file
  write_me();
  read_me();

  return 0;
}