CC=gcc
CFLAGS= -O0 -ggdb -Wall -Wextra -mno-red-zone -lrt -lpthread

.SILENT: clean

all: server client

server: server.c shmem.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c shmem.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f server client tmp.txt