CC=gcc
CFLAGS= -O3 -Wall -Wextra -Wno-unused-result -Wno-unused-variable -lpthread

SYMLIB_DIR=../../../Symlib
SYMLIB_LINK=-lSym
SYMLIB_INCLUDE=$(SYMLIB_DIR)/include
SYMLIB_LIB_DIR=$(SYMLIB_DIR)/dynam_build

all: old_client old_client_elevated client_elevated client server server_elevated serverkill old_server old_server_elevated

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

old_server: server.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK)

old_server_elevated: server.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D ELEVATED_MODE

old_client: old_client.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D INDEPENDENT_CLIENT

old_client_elevated: old_client.c common.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D ELEVATED -D INDEPENDENT_CLIENT

clean:
	rm -rf client_elevated client server server_elevated serverkill old_server old_server_elevated old_client *.txt old_client_elevated



