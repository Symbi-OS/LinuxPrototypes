#include <stdio.h>
extern unsigned long page_offset_base;
int main() {
    printf("page_offset_base: %ld\n", page_offset_base);
    return 0;
}