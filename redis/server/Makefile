# Copying this redis junk out of the Symbi-OS makefile.


REDIS_CMD=../../../artifacts/redis/fed36/redis-server --protected-mode no --save '' --appendonly no
# 0 seems pathological. Worst observed performance
TASKSET_CMD=taskset -c 1 bash -c

run_redis:
	${TASKSET_CMD} '${REDIS_CMD}' 

run_redis_passthrough:
	${TASKSET_CMD} 'shortcut.sh -p --- ${REDIS_CMD}' 

run_redis_interpose:
	${TASKSET_CMD} 'shortcut.sh --- ${REDIS_CMD}' 

run_redis_elev:
	${TASKSET_CMD} 'shortcut.sh -be --- ${REDIS_CMD}' 

run_redis_sc_write:
	${TASKSET_CMD} 'shortcut.sh -be -s "write->__x64_sys_write" --- ${REDIS_CMD}' 

run_redis_sc_read:
	${TASKSET_CMD} 'shortcut.sh -be -s "read->__x64_sys_read" --- ${REDIS_CMD}' 

#run_redis_sc_rw:
#	${TASKSET_CMD} 'shortcut.sh -be -s "write->__x64_sys_write" -s "read->__x64_sys_read" --- ${REDIS_CMD}' 

run_redis_sc_rw:
	${TASKSET_CMD} 'shortcut.sh -be -s "epoll_wait->__x64_sys_epoll_wait" -s "write->__x64_sys_write" -s "read->__x64_sys_read" --- ${REDIS_CMD}' 

run_redis_tcp:
	${TASKSET_CMD} 'shortcut.sh -be -s "write->tcp_sendmsg" --- ${REDIS_CMD}' 

prepare:
	ip addr add 192.168.122.238/24 dev enp1s0
	ip link set up enp1s0
	sudo systemctl mask systemd-journald
	sudo systemctl stop systemd-journald
	sudo systemctl stop systemd-udevd
	. prep_envt.sh
	mitigate all
cmd:
	LD_LIBRARY_PATH=/home/sym/Symbi-OS/Symlib/dynam_build  BEGIN_ELE=1 SHORTCUT_write_TO_ksys_write=1 SHORTCUT_read_TO_ksys_read=1 LD_PRELOAD=/home/sym/Symbi-OS/Tools/bin/shortcut/sc_lib.so  artifacts/redis/redis-server --protected-mode no --save  --appendonly no

fix:
	sudo systemctl unmask systemd-journald
	sudo systemctl start systemd-journald

#taskset -c 0 bash -c shortcut.sh -p --- ./LinuxPrototypes/getpid/getpid 1000000
run_tcp:
	taskset -c 1 bash -c 'shortcut.sh -be -s "read->tcp_recvmsg" -s "write->tcp_sendmsg" --- ~/Symbi-OS/artifacts/redis/fed36/redis-server --protected-mode no --save '' --appendonly no'
