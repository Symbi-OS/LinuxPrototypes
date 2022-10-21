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

int main() {
	clock_t start, end;
	double cpu_time_used = 0;

	FILE* log = fopen("stress_test_log", "w");
	int logfd = fileno(log);

	// Start performance timer
	start = clock();

	// Begin stress testing
	for (int i = 0; i < STRESS_TEST_ITERATIONS; ++i) {
		write(logfd, "ksys_write\r", 11);
	}

	// Stop performance timer
	end = clock();

	// Print the results
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Time used: %f\n", cpu_time_used);

	return 0;
}
