CC=gcc
CFLAGS= -O0 -ggdb -Wall -Wextra -mno-red-zone -lrt -lpthread
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

KALLSYMDIR=../../../kallsymlib/
KALLSYMLIB=../../../kallsymlib/libkallsym.a

DEPENDENCY_LIBS=$(KALLSYMLIB) $(SYMLIB)
export KALLSYMDIR KALLSYMLIB DEPENDENCY_LIBS

.SILENT: clean server server_elevated client independent independent_elevated

all: server_elevated server client independent independent_elevated

server: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB) -I $(SYMLIB_INCLUDE_DIR)

server_elevated: server.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB) -I $(SYMLIB_INCLUDE_DIR) -D ELEVATED_MODE

client: client.c
	gcc client.c -o client

independent: independent_write.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB) -I $(SYMLIB_INCLUDE_DIR)

independent_elevated: independent_write.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB) -I $(SYMLIB_INCLUDE_DIR) -D ELEVATED_MODE

clean:
	rm -rf *~
	rm -f server client server_elevated independent independent_elevated tmp.txt
	touch tmp.txt

