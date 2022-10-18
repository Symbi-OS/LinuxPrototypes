#ifndef NETUTILS_H
#define NETUTILS_H
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>

int net_connect(const char* host, int port);

int net_bind(const char* host, int port);

int net_listen(int connections);

int net_accept();

void net_close();

int net_set_blocking(int blocking);

size_t net_recv_bytes(void* buffer, size_t len);

size_t net_send_bytes(void* buffer, size_t len);

#endif
