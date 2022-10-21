#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdio.h>

//doing 30K writes directly and measure the time elapse.
int main()
{
    char *filename = "tmp.txt";
    clock_t start, end;
    double cpu_time_used;
    
    // open the file for writing
    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd == -1)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
    // write to the text file

    start = clock();
    for (int i = 0; i < 30000; i++){
        write(fd, "aaaaaaaaaa",10);
    }

    end = clock();
    cpu_time_used = (double) (end - start);

    printf("time: %f\n", cpu_time_used/CLOCKS_PER_SEC);
    return 0;
}
