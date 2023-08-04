#include <stdio.h>

extern void * tu_main; 

int main() {
    // printf("page_offset_base: %ld\n", page_offset_base);
    printf("The first bytes of text are at %p\n", main );
    printf("The first 8 bytes are %ld\n", *(long*)main);

    printf("tu_main is %p\n", &tu_main);
    printf("The first 8 bytes are %ld\n", *(long*)&tu_main);

    return 0;
}