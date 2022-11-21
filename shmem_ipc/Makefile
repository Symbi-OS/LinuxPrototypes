CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt
export CC CFLAGS

#Check for kernel version
UNAME=$(shell uname -r)

ifeq ($(TYPE), static)
	SYMLIB_LINK=$(SYMLIB_STATIC)
endif

RUN_ITERATIONS := 2000000
export RUN_ITERATIONS

all: server client independent_client expe_info

# GET TARGET INFO

SYS_INFO="sys_info/"

info:
	mkdir -p $(SYS_INFO)

info_cmdline:
	cat /proc/cmdline > $(SYS_INFO)cmdline.info

info_ifconfig:
	ifconfig > $(SYS_INFO)ifconfig.info

info_nproc:
	nproc > $(SYS_INFO)nproc.info

info__uptime:
	uptime > $(SYS_INFO)uptime.info

info_cpu:
	cat /proc/cpuinfo > $(SYS_INFO)cpu.info

info_uname:
	uname -srm > $(SYS_INFO)uname.info

info_ps:
	echo "NPROC: " > $(SYS_INFO)ps.info
	ps aux | wc -l; ps aux >> $(SYS_INFO)ps.info

expe_info: info info_cmdline info_ifconfig info_nproc info__uptime info_cpu info_uname info_ps

server: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

independent_client: client.c
	$(CC) $(CFLAGS) $^ -o $@ -D INDEPENDENT_CLIENT

run_independent_client: independent_client
	taskset -c 1 ./independent_client $(RUN_ITERATIONS)

run_client: independent_client
	taskset -c 1 bash -c 'LD_PRELOAD=/home/ar/Symbi-OS/Tools/bin/ipc_shortcut/ipc_shortcut.so ./independent_client $(RUN_ITERATIONS)'

run_server: server
	taskset -c 0 ./server $(RUN_ITERATIONS)

run_server_elevated: server
	taskset -c 0 bash -c 'shortcut.sh -be -s "write->ksys_write" -- ./server $(RUN_ITERATIONS)'

clean:
	rm -rf server client independent_client run_log scalability.log
