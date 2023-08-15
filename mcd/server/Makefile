# Common Variables
USER = sym
THREADS = 1
IP = 172.16.97.208
BINARY = binary
PORT = 18080
TASKSET = taskset -c 1
MEMCACHED = memcached

# Targets
run:
	$(TASKSET) $(MEMCACHED) -u $(USER) -t $(THREADS) -l $(IP) -B $(BINARY) -p $(PORT)

run_sc:
	${TASKSET} 'shortcut.sh -be  -s "read->__x64_sys_read" --- ${MEMCACHED} -u ${USER} -t ${THREADS} -l ${IP} -B ${BINARY} -p ${PORT}'

.PHONY: run
