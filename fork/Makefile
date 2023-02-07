# all and clean are phony targets
.PHONY: all run clean

all: fork

fork: fork.c
	gcc fork.c -o fork

run:
	./fork

run_elev:
	taskset -c 0 shortcut.sh -be --- ./fork


# Works fine

run_share_core:
	taskset -c 0 ./fork  &
	sleep 1
	taskset -c 0 ./fork 1

run_elev_share_core:
	taskset -c 0 shortcut.sh -be --- ./fork  &
	sleep 1
	taskset -c 0 ./fork 1

run_shortcut:
	taskset -c 0 shortcut.sh -be -s "fork->__do_sys_fork" --- ./fork  &


run_shortcut_share_core:
	taskset -c 0 shortcut.sh -be -sc "fork->__do_sys_fork" --- ./fork  &
	sleep 1
	taskset -c 0 ./fork 1

run_s:
	taskset -c 0 'shortcut.sh -be -s "fork->__do_sys_fork" --- ./fork 4'
# taskset -c 0 bash -c 'shortcut.sh -be -s "fork->__do_sys_fork" --- ./LinuxPrototypes/fork/fork 3000'
clean:
	rm -f fork
