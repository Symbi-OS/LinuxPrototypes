#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#if 0
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
#endif

// Weak function print_cr3
void print_cr3(){
  printf("print_cr3 not implemented\n");
}

int main(int argc, char *argv[]){
    // check if parent or child
    // Print cr3

    if (fork() == 0){
        // child
        printf("Child: %d\n", getpid());
    } else {
        // parent
        printf("Parent: %d\n", getpid());
    }
    
    return 0;
}