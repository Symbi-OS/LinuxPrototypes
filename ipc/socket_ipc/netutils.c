#include "netutils.h"
#include <string.h>

// Uncomment this line if you want to print debug
// info about the client-server connection.
//#define DBGPRINT_NETWORK_CONNECTION

#ifdef DBGPRINT_NETWORK_CONNECTION
	#define dbg_print(fmt, ...) dbg_print(fmt, ##__VA_ARGS__)
#else
	#define dbg_print
#endif

static int listenfd = 0, connfd = 0;
static struct sockaddr_in serv_addr;

int net_connect(const char* host, int port) {
	connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (connfd < 0) {
		dbg_print("Failed to create socket");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
		dbg_print("Invalid host address: %s\n", host);
		return -1;
	}

	int result = connect(connfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (result < 0) {
		dbg_print("Failed to connect to server %s:%i\n", host, port);
		return -1;
	}

	return 0;
}

int net_bind(const char* host, int port) {
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (connfd < 0) {
        dbg_print("Failed to create listening socket");
        return -1;
    }

	int reuse;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(int)) == -1){
 		dbg_print("Failed to make the socket reusable\n");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &serv_addr.sin_addr) <= 0) {
        dbg_print("Invalid host address: %s\n", host);
        return -1;
    }

	int result = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (result < 0) {
		dbg_print("Failed to bind listening socket on port %i\n", port);
		return -1;
	}

	return 0;
}

int net_listen(int connections) {
	if (listen(listenfd, connections) < 0) {
		dbg_print("Failed to listen for connections\n");
		return -1;
	}

	return 0;
}

int net_accept() {
	socklen_t clilen = sizeof(serv_addr);
	connfd = accept(listenfd, (struct sockaddr*)&serv_addr, (socklen_t*)&clilen);
	if (connfd < 0) {
		dbg_print("Failed to accept client connection\n");
		return -1;
	}
	
	return 0;
}

void net_close() {
	close(listenfd);
	close(connfd);
}

int net_set_blocking(int blocking) {
	int flags = fcntl(connfd, F_GETFL, 0);
	if (flags == -1) {
		dbg_print("Error retrieving current socket blocking state\n");
		return -1;
	}

	flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

	int result = fcntl(connfd, F_SETFL, flags);
	if (result == -1) {
		dbg_print("Failed to set socket blocking state\n");
		return -1;
	}

	return 0;
}

size_t net_recv_bytes(void* buffer, size_t len) {
	return recv(connfd, buffer, len, 0);
}

size_t net_send_bytes(void* buffer, size_t len) {
	return send(connfd, buffer, len, 0);
}

