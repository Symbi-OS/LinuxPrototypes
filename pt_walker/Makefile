CC=gcc
CFLAGS=-O0 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt

SYMLIB_DIR=../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym
TX86_INC=/home/ar/JAPlay/Om/trivalX86/apps/S2
CFLAGS += -I${TX86_INC} -D__KERNEL__

LINUX_PATH=../../linux
obj-m += modtest.o

# all: pt_walker
all: pgm

build_all:
	make modtest
	make pt_walker

pt_walker: pt_walker.o
	gcc -o $@ $^ -L$(SYMLIB_DYNAM_BUILD_DIR) $(SYMLIB_LINK) -lc

pt_walker.o: pt_walker.c
	$(CC) $(CFLAGS) $^ -c $@ -I$(SYMLIB_INCLUDE_DIR)

jatest: jatest.c
	$(CC) $(CFLAGS) $^ -o $@ -I$(SYMLIB_INCLUDE_DIR) -L$(SYMLIB_DYNAM_BUILD_DIR) $(SYMLIB_LINK)

modtest: modtest.c
	make -C $(LINUX_PATH) M=$(PWD) modules

modtest.o: modtest.c
	-make modtest

pgm: pt_walker.o modtest.o
	gcc -o pgm modtest.o pt_walker.o -lSym -L../../Symlib/dynam_build -Xlinker --defsym=boot_cpu_data=0xffffffff82b63640 -Xlinker --defsym=physical_mask=0xffffffff82708310 -Xlinker --defsym=sme_me_mask=0xffffffff8284def8 -Xlinker --defsym=pv_ops=0xffffffff8283cc40 -Xlinker --defsym=page_offset_base=0xffffffff826f1a40 -Xlinker --defsym=pgdir_shift=0xffffffff826f1a4c -Xlinker --defsym=vmemmap_base=0xffffffff826f1a30 -Xlinker --defsym=ioremap=0xffffffff8107d260

walkmod: walkmod.c
	make -C $(LINUX_PATH) M=$(PWD) modules

run: pt_walker
	./pt_walker

clean:
	rm -rf pt_walker jatest .* Module.* modules.* modtest *.ko *.o *.mod* pgm
