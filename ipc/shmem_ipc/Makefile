CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -Wno-unused-parameter
export CC CFLAGS

SYMLIB_DIR=../../../Symlib
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
export SYMLIB_DIR SYMLIB_INCLUDE_DIR SYMLIB_LINK

LIBRARY_PATH=$(SYMLIB_DYNAM_BUILD_DIR)
export LIBRARY_PATH

all: client_no_work client_work server_no_work server_work server_work_elevated independent_client

server_no_work: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

server_work: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ -D DO_WORK

server_work_elevated: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D DO_WORK -D ELEVATED

client_no_work: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

client_work: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ -D DO_WORK

independent_client: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ -D INDEPENDENT_CLIENT

clean:
	rm -rf client_* server_* independent_client log results.csv
