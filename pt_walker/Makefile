CC=gcc
CFLAGS=-O0 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt

SYMLIB_DIR=../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym
# CFLAGS += -D__KERNEL__

LINUX_PATH=../../linux
obj-m += modtest.o

all: pt_walker
# all: pgm

build_all:
	make modtest
	make pt_walker

pt_walker: pt_walker.o
	gcc -o $@ $^ -L$(SYMLIB_DYNAM_BUILD_DIR) $(SYMLIB_LINK)

pt_walker.o: pt_walker.c
	$(CC) $(CFLAGS) $^ -c $@ -I$(SYMLIB_INCLUDE_DIR)

modtest: modtest.c
	make -C $(LINUX_PATH) M=$(PWD) modules

modtest.o: modtest.c
	-make modtest

pgm: pt_walker.o modtest.o
	gcc -o pgm modtest.o pt_walker.o -lSym -L../../Symlib/dynam_build

run: pt_walker
	./pt_walker

clean:
	rm -rf pt_walker jatest .* Module.* modules.* modtest *.ko *.o *.mod* pgm
