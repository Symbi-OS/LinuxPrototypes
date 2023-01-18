// A program to demonstrate the use of variadic functions
#include <stdarg.h>
#include <stdio.h>

// This function takes variable number of arguments
void foo(int arg, ...)
{
    va_list ap;
    int i;

    // Initialize the argument list
    va_start(ap, arg);

    // Access all the arguments assigned to ap
    for (i = arg; i != 0; i = va_arg(ap, int))
        printf("%d ", i);

    // Clean up the list
    va_end(ap);
}
// Main calls foo
int main()
{
    foo(1, 2, 3, 4, 5, 0);
    return 0;
}