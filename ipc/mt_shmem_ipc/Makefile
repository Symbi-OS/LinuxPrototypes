CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lrt -lpthread
BACKFILE = "process_shared_memory"

.SILENT: clean

all: server client

server: server.c shmem.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c shmem.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f server client *.txt
	rm -f /dev/shm/$(BACKFILE)