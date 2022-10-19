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

.SILENT: clean

all: memwriter memreader

memwriter: memwriter.c
	$(CC) $(CFLAGS) -o memwriter memwriter.c $(SYMLIB_LINK) $(KALLSYMLIB) -I $(SYMLIB_INCLUDE_DIR)

memreader: memreader.c
	gcc -o memreader memreader.c -lrt -lpthread

clean:
	rm -rf memreader memwriter *\~
