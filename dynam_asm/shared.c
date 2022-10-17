#include <stdio.h>

/* int bar(){ */
/*   return 42; */
/* } */

extern int bar();
void shared_lib_fn(){
  printf("in %s\n", __func__);
  printf("bar returned %d\n", bar());
}
