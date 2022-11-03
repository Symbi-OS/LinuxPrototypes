CC=gcc
CFLAGS= -O3 -Wall -Wextra -Wno-unused-result -Wno-unused-variable -lpthread

SYMLIB_DIR=../../../Symlib
SYMLIB_LINK=-lSym
SYMLIB_INCLUDE=$(SYMLIB_DIR)/include
SYMLIB_LIB_DIR=$(SYMLIB_DIR)/dynam_build

all: client_elevated client server server_elevated serverkill

client_elevated: client.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D ELEVATED

client: client.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@

server: server.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@

serverkill: serverkill.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@

server_elevated: server.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D ELEVATED

clean:
	rm -rf client_elevated client server server_elevated serverkill tmp.txt



