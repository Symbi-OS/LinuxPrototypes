# all and clean are phony targets
.PHONY: all run clean

all: getpid

getpid: getpid.c
	gcc getpid.c -o getpid

run:
	./getpid 10000000

# Works fine
run_share_core:
	taskset -c 0 ./getpid 40000000 &
	sleep 1
	taskset -c 0 ./getpid 1

run_elev_share_core:
	taskset -c 0 shortcut.sh -be --- ./getpid 40000000 &
	sleep 1
	taskset -c 0 ./getpid 1

run_shortcut_share_core:
	taskset -c 0 shortcut.sh -be -sc "getpid->__do_sys_getpid" --- ./getpid 40000000 &
	sleep 1
	taskset -c 0 ./getpid 1

run_s:
	taskset -c 0 'shortcut.sh -be -s "getpid->__do_sys_getpid" --- ./getpid 4'
# taskset -c 0 bash -c 'shortcut.sh -be -s "getpid->__do_sys_getpid" --- ./LinuxPrototypes/getpid/getpid 3000'
clean:
	rm -f getpid
