CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lrt -lpthread
export CC CFLAGS

SYMLIB_DIR=../../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_STATIC=$(SYMLIB_DIR)/build/libsym.a
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym

ifeq ($(TYPE), static)
	SYMLIB_LINK=$(SYMLIB_STATIC)
endif

export SYMLIB_DIR SYMLIB_STATIC SYMLIB_INCLUDE_DIR SYMLIB_DYNAM_BUILD_DIR SYMLIB_LINK

LIBRARY_PATH=$(SYMLIB_DYNAM_BUILD_DIR)
export LIBRARY_PATH

all: server server_elevated client independent_client independent_client_elevated

server: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

server_elevated: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D ELEVATED_MODE

client: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

independent_client: client.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D INDEPENDENT_CLIENT

independent_client_elevated: client.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D INDEPENDENT_CLIENT -D ELEVATED_MODE

independent_client_shortcut: independent_client.c
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):/home/sym/Symbi-OS/Tools/bin/shortcut/ LD_PRELOAD="sc_lib.so" $(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

clean:
	rm -rf server server_elevated client independent_client stress_test_log \
		independent_client_elevated
