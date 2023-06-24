CC=gcc
CFLAGS=-O0 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt

SYMLIB_DIR=../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym
TX86_INC=/home/ar/JAPlay/Om/trivalX86/apps/S2
CFLAGS += -I${TX86_INC}

LINUX_PATH=../../linux
obj-m += modtest.o

all: pt_walker

build_all:
	make modtest
	make pt_walker

pt_walker: pt_walker.o modtest.o
	gcc -o $@ $^ -L$(SYMLIB_DYNAM_BUILD_DIR) $(SYMLIB_LINK) -lc

pt_walker.o: pt_walker.c
	$(CC) $(CFLAGS) $^ -c $@ -I$(SYMLIB_INCLUDE_DIR)

jatest: jatest.c
	$(CC) $(CFLAGS) $^ -o $@ -I$(SYMLIB_INCLUDE_DIR) -L$(SYMLIB_DYNAM_BUILD_DIR) $(SYMLIB_LINK)

modtest: modtest.c
	make -C $(LINUX_PATH) M=$(PWD) modules

run: pt_walker
	./pt_walker

clean:
	rm -rf pt_walker jatest .* Module.* modules.* modtest *.ko *.o *.mod*
