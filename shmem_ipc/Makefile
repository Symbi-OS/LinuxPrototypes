CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt
export CC CFLAGS

SYMLIB_DIR=../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_STATIC=$(SYMLIB_DIR)/build/libsym.a
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym

#Check for kernel version
UNAME=$(shell uname -r)

ifeq ($(TYPE), static)
	SYMLIB_LINK=$(SYMLIB_STATIC)
endif

ifeq ($(UNAME), 5.14.0-symbiote+)
all: server client independent_client server_elevated independent_client_elevated
else
all: server client independent_client
endif

export SYMLIB_DIR SYMLIB_STATIC SYMLIB_INCLUDE_DIR SYMLIB_DYNAM_BUILD_DIR SYMLIB_LINK

LIBRARY_PATH=$(SYMLIB_DYNAM_BUILD_DIR)
export LIBRARY_PATH

server: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

server_elevated: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D ELEVATED_MODE

client: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

independent_client: client.c
	$(CC) $(CFLAGS) $^ -o $@ -D INDEPENDENT_CLIENT

independent_client_elevated: client.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D INDEPENDENT_CLIENT -D ELEVATED_MODE

clean:
	rm -rf server server_elevated client independent_client* run_log
