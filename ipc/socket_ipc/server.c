#include "netutils.h"
#include <string.h>

const char* HOST = "0.0.0.0";
const int PORT = 4500;

#define CLIENT_DATA_BUFFER_SIZE 512

#define STRESS_TEST_LOOP_COUNT 200000 // 200,000

int main() {
	printf("Starting the server %s:%i...\n", HOST, PORT);
	if (net_bind(HOST, PORT)) {
		return -1;
	}

	net_listen(1);
	net_accept();
	printf("[+] Connection established [+]\n\n");

	char client_data_buffer[CLIENT_DATA_BUFFER_SIZE] = { 0 };
	const char* response = "server response";

	for (int i = 0; i < STRESS_TEST_LOOP_COUNT; ++i) {
		net_recv_bytes(client_data_buffer, CLIENT_DATA_BUFFER_SIZE);
		int status = net_send_bytes((void*)response, strlen(response));

		if (status <= 0) {
			break;
		}
	}

	net_close();
	printf("Connection closed\n");
	return 0;
}
