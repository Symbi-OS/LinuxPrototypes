CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lrt -lpthread
BACKFILE = "process_shared_memory"

SYMLIB_DIR=../../../Symlib
SYMLIB_LINK=-lSym
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_STATIC=$(SYMLIB_DIR)/build/libsym.a
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include

LIBRARY_PATH=$(SYMLIB_DYNAM_BUILD_DIR)

export SYMLIB_DIR SYMLIB_STATIC SYMLIB_INCLUDE_DIR SYMLIB_DYNAM_BUILD_DIR SYMLIB_LINK
export LIBRARY_PATH


KALLSYMDIR=../../../kallsymlib/
KALLSYMLIB=../../../kallsymlib/libkallsym.a

DEPENDENCY_LIBS=$(KALLSYMLIB) $(SYMLIB)
export KALLSYMDIR KALLSYMLIB DEPENDENCY_LIBS

.SILENT: clean

all: server elevated_server client serverkill

server: server.c shmem.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE_DIR) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB)

elevated_server: server.c shmem.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE_DIR) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB) -D ELEVATED

client: client.c shmem.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE_DIR) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB)

serverkill: serverkill.c shmem.c
	$(CC) $(CFLAGS) -I $(SYMLIB_INCLUDE_DIR) $^ -o $@ $(SYMLIB_LINK) $(KALLSYMLIB)

clean:
	rm -f server client *.txt serverkill elevated_server
	rm -f /dev/shm/$(BACKFILE)