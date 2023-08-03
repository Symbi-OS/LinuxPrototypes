CC=gcc
CFLAGS=-O0 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt

SYMLIB_DIR=../../../Symlib
SYMLIB_DYNAM_BUILD_DIR=$(SYMLIB_DIR)/dynam_build
SYMLIB_INCLUDE_DIR=$(SYMLIB_DIR)/include
SYMLIB_LINK=-lSym
CFLAGS += -D__KERNEL__

LINKER_KERNEL_SYMDEFS = -Xlinker --defsym=pgdir_shift=0xffffffff826f1a6c -Xlinker --defsym=boot_cpu_data=0xffffffff82b63640 -Xlinker --defsym=physical_mask=0xffffffff82708310 -Xlinker --defsym=sme_me_mask=0xffffffff8284def8 -Xlinker --defsym=ptrs_per_p4d=0xffffffff826f1a68 -Xlinker --defsym=pv_ops=0xffffffff8283cc40 -Xlinker --defsym=page_offset_base=0xffffffff826f1a60

LINUX_PATH=../../../linux
obj-m += kmod.o

all: pgm

user.o: user.c
	$(CC) $(CFLAGS) -c $^ -o $@ -I$(SYMLIB_INCLUDE_DIR)

kmod: kmod.c
	make -C $(LINUX_PATH) M=$(PWD) modules

kmod.o: kmod.c
	-make kmod

pgm: user.o kmod.o
	# $(CC) $(CFLAGS) -o $@ kmod.o user.o -lSym -L../../../Symlib/dynam_build $(LINKER_KERNEL_SYMDEFS)
	$(CC) $(CFLAGS) -o $@ kmod.o user.o -lSym -L../../../Symlib/dynam_build -T symhelper.ld

pgm_dynam: user.o kmod.o
	$(CC) $(CFLAGS) -o $@ kmod.o user.o -lSym -L../../../Symlib/dynam_build -L./ -lkallsyms

pgm_dynam_working: user.o kmod.o
	$(CC) $(CFLAGS) -o $@ kmod.o user.o -lSym -L../../../Symlib/dynam_build -L./ -lkallsyms -u page_offset_base -u pgdir_shift -u boot_cpu_data -u physical_mask -u sme_me_mask -u ptrs_per_p4d -u pv_ops

compilation_test: pgm pgm_dynam pgm_dynam_working
	nm pgm | grep page_offset_base
	nm pgm_dynam | grep page_offset_base
	nm pgm_dynam_working | grep page_offset_base

run: pgm
	./pgm

clean:
	rm -rf .Module.* .modules.* Module* modules* kmod kmod.mod* *.ko *.o .kmod* pgm
