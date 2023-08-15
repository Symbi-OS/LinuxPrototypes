# Common Variables
USER = sym
THREADS = 1
IP = 172.16.97.208
BINARY = binary
PORT = 18080
TASKSET = taskset -c 1
MEMCACHED = memcached
MEMCACHED_CMD = $(MEMCACHED) -u $(USER) -t $(THREADS) -l $(IP) -B $(BINARY) -p $(PORT)

# Default target
.PHONY: run run_be run_sc
run:
	$(TASKSET) $(MEMCACHED_CMD)

# Run with interposition
run_int:
	$(TASKSET) bash -c 'shortcut.sh --- $(MEMCACHED_CMD)'

# Run with elevated privilege
run_be:
	$(TASKSET) bash -c 'shortcut.sh -be --- $(MEMCACHED_CMD)'

# Run with syscall shortcutting
run_sc:
	$(TASKSET) bash -c 'shortcut.sh -be \
	-s "read->__x64_sys_read" \
	-s "sendmsg->__x64_sys_sendmsg" \
	--- $(MEMCACHED_CMD)'
