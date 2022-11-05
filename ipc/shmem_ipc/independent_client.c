#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include "LINF/sym_all.h"

#define STRESS_TEST_ITERATIONS 200000

typedef int(*ksys_write_t)(unsigned int fd, const char *buf, size_t count);

int main() {
	clock_t start, end;
	double cpu_time_used = 0;

	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);

#ifdef ELEVATED_MODE
    // Init symbiote library and kallsymlib
    sym_lib_init();

    // Get the address of ksys_write
    ksys_write_t my_ksys_write = (ksys_write_t)sym_get_fn_address("ksys_write");

    // Elevate
    sym_elevate();
#endif

	// Start performance timer
	start = clock();

	// Begin stress testing
	for (int i = 0; i < STRESS_TEST_ITERATIONS; ++i) {
#ifdef ELEVATED_MODE
		if (i % 20 == 0) {
            write(logfd, "ksys_write\r", 11);
        } else {
    		my_ksys_write(logfd, "ksys_write\r", 11);
		}
#else
		write(logfd, "ksys_write\r", 11);
#endif
	}

	// Stop performance timer
	end = clock();

#ifdef ELEVATED_MODE
	sym_lower();
#endif

	// Print the results
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time used: %f\n", cpu_time_used);

	return 0;
}
