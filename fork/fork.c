#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>


uint64_t get_cr3(){
  uint64_t my_cr3;
  asm volatile("mov %%cr3, %0" : "=r"(my_cr3));
  return my_cr3;
}

int main(int argc, char *argv[]){
    fprintf(stderr, "About to fork, cr3 is %lx\n", get_cr3());

    int forkid = fork();

    if (forkid == 0){
        // child
        // print child cr3
        fprintf(stderr, "Child CR3: %lx\n", get_cr3());
    } else {
        // parent
        fprintf(stderr, "Parent CR3: %lx\n", get_cr3());
    }
    
    return 0;
}