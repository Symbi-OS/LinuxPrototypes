#include "netutils.h"
#include <string.h>   // for strlen
#include <sys/time.h> // for measuring performance

const char* HOST = "127.0.0.1";
const int PORT = 4500;

#define RECV_BUFFER_SIZE 512

#define STRESS_TEST_LOOP_COUNT 200000 // 200,000

void stress_test(int loop_cap) {
	// Used to track the number of valid responses that were received
	int responses_received = 0;

	char preallocated_recv_buffer[RECV_BUFFER_SIZE] = { 0 };
	const char* echo = "echo";

	// Start measuring time
    struct timeval begin, end;
    gettimeofday(&begin, 0);

	for (int i = 0; i < loop_cap; ++i) {
		//
		// Sending an echo to the server
		//
    		net_send_bytes((void*)echo, strlen(echo));

		//
		// Receiving a response from the server
		//
		int bytes_received = net_recv_bytes((void*)preallocated_recv_buffer, RECV_BUFFER_SIZE);
		if (bytes_received > 0) {
			++responses_received;
		} else {
			break;
		}
	}

	// Stop measuring time and calculate the elapsed time
 	gettimeofday(&end, 0);
  	long seconds = end.tv_sec - begin.tv_sec;
  	long microseconds = end.tv_usec - begin.tv_usec;
 	double elapsed = seconds + microseconds*1e-6;

	printf("Received responses: %i / %i\n\n", responses_received, loop_cap);
    printf("Time measured: %.8f seconds.\n", elapsed);
}

int main() {
	printf("Connecting client to %s:%i...\n", HOST, PORT);
	if (net_connect(HOST, PORT)) {
		return -1;
	}
	printf("Connected to server!\n");

	stress_test(STRESS_TEST_LOOP_COUNT);

	net_close();
	printf("Disconnected from the server\n");
	return 0;
}
