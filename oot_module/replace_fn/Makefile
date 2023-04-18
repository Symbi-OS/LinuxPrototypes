obj-m += kelevate_module.o

MOD_PATH=~/Symbi-OS/linux
# Terminate on first error
# CFLAGS=-Werror -Wall
all:
	make -C $(MOD_PATH) M=$(PWD) modules

clean:
	make -C $(MOD_PATH) M=$(PWD) clean
