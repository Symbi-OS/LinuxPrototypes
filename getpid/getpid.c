#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int bench_time(int count){
  clock_t start, end;
  double cpu_time_used;
  start = clock();
  /* Do the work. */
  int i;
  for(i=0; i<count; i++)
    getpid();

  end = clock();
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("Time used: %f\n", cpu_time_used);

  return 0;
}
int main(int argc, char *argv[]){
// First arg is count
    if (argc < 2) {
        printf("Usage: %s <count>\n", argv[0]);
        exit(1);
    }

    int count = atoi(argv[1]);

    bench_time(count);
    return 0;
}