# all and clean are phony targets
.PHONY: all run clean profile_pt profile_interpose profile_all

all: getpid

getpid: getpid.c
	gcc -O3	getpid.c -o getpid

# Chosen to take 1 sec.
ITERATIONS=160000000
WORKLOAD=./getpid ${ITERATIONS}
TASKSET_CMD=taskset -c 0 bash -c

run: getpid
	./getpid ${ITERATIONS}

run_pt: getpid
	${TASKSET_CMD} 'shortcut.sh -p --- ${WORKLOAD}'

run_interpose: getpid
	${TASKSET_CMD} 'shortcut.sh --- ${WORKLOAD}'

run_elev: getpid
	${TASKSET_CMD} 'shortcut.sh -be --- ${WORKLOAD}'

run_shortcut: getpid
	${TASKSET_CMD} 'shortcut.sh -be -s "getpid->__x64_sys_getpid" --- ${WORKLOAD}'

profile_pt: getpid
	./profile.sh "'shortcut.sh -p --- ${WORKLOAD}'" getpid_pt.svg

profile_interpose: getpid
	./profile.sh "'shortcut.sh --- ${WORKLOAD}'" getpid_interpose.svg

profile_all: profile_pt profile_interpose


clean:
	rm -f getpid *.svg
