#include <stdio.h>
#include "shared.h"

int main(){
  printf("in main\n");
  shared_lib_fn();
  return 0;
}
