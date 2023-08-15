# Common Variables
SERVER = 172.16.97.208
THREADS = --threads 4
PIPELINE = 1
TASKSET = taskset -c 1,2,3,4
REDIS_BENCHMARK = ./redis/src/redis-benchmark
TESTS = get

# Targets
warmup: REQUESTS = 1000000
warmup: CONN = 150
warmup:
	@echo "warmup"
	@$(TASKSET) $(REDIS_BENCHMARK) $(THREADS) -q -n $(REQUESTS) -c $(CONN) -h $(SERVER) -t $(TESTS) -P $(PIPELINE)

expt: REQUESTS = 8000000
expt: CONN = 200
expt:
	@echo "expt"
	@$(TASKSET) $(REDIS_BENCHMARK) $(THREADS) -q -n $(REQUESTS) -c $(CONN) -h $(SERVER) -t $(TESTS) -P $(PIPELINE)

all: warmup expt

.PHONY: warmup expt all
