CC=gcc
CFLAGS=-O0 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt

LINUX_PATH=../../../linux
obj-m += walkmod.o

all: walkmod

walkmod: walkmod.c
	make -C $(LINUX_PATH) M=$(PWD) modules

run:
	sudo insmod walkmod.ko
	sudo rmmod walkmod.ko

clean:
	rm -rf .Module.* .modules.* walkmod *.ko *.o *.mod* .walkmod* Module* modules*
