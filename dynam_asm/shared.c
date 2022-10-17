#include <stdio.h>
#include "shared.h"

void shared_lib_fn(){
  printf("in %s\n", __func__);
  printf("bar returned %d\n", bar());
  printf("bar lives at %p\n", &bar);
}
