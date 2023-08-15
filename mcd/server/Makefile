# Common Variables
CORE = 1
USER = sym
THREADS = 1
LISTEN = 172.16.97.208
BINARY = binary
PORT = 18080
TASKSET = taskset -c
MEMCACHED = memcached

# Targets
run:
	$(TASKSET) $(CORE) $(MEMCACHED) -u $(USER) -t $(THREADS) -l $(LISTEN) -B $(BINARY) -p $(PORT)

.PHONY: run
