# Common Variables
SERVER = 172.16.97.208
THREADS = --threads 4
PIPELINE = 1
TASKSET = taskset -c 1,2,3,4,5,6
REDIS_BENCHMARK = ~/redis/src/redis-benchmark
TESTS = get

all: warmup expt

# Targets
warmup: REQUESTS = 1000000
warmup: CONN = 50
warmup:
	@echo "warmup"
	@$(TASKSET) $(REDIS_BENCHMARK) $(THREADS) -q -n $(REQUESTS) -c $(CONN) -h $(SERVER) -t $(TESTS) -P $(PIPELINE)

expt: REQUESTS = 8000000
expt: CONN = 200
expt:
	@echo "expt"
	@$(TASKSET) $(REDIS_BENCHMARK) $(THREADS) -q -n $(REQUESTS) -c $(CONN) -h $(SERVER) -t $(TESTS) -P $(PIPELINE)

# Memtier-specific Variables
MEMTIER_BENCHMARK = ~/memtier_benchmark/memtier_benchmark
MEMTIER_PORT = 6379
MEMTIER_PROTOCOL = redis
MEMTIER_CONNECTIONS = 25
# MEMTIER_CONNECTIONS = 28
MEMTIER_RAND = 1000
MEMTIER_RATIO = 0:1
MEMTIER_DATA_SIZE = 1
MEMTIER_REQUESTS = 20000
# MEMTIER_THREADS = 4
# couldn't do better
#MEMTIER_THREADS = 2
MEMTIER_THREADS = 6

OUTPUT_FILE = output.txt

memtier:
	@echo Warmup:
	$(TASKSET) $(MEMTIER_BENCHMARK) -s $(SERVER) -p $(MEMTIER_PORT) -P $(MEMTIER_PROTOCOL) -t $(MEMTIER_THREADS) -c $(MEMTIER_CONNECTIONS) -n 5000 -R $(MEMTIER_RAND) --ratio=$(MEMTIER_RATIO) -d $(MEMTIER_DATA_SIZE)
	@echo Workload:
	$(TASKSET) $(MEMTIER_BENCHMARK) -s $(SERVER) -p $(MEMTIER_PORT) -P $(MEMTIER_PROTOCOL) -t $(MEMTIER_THREADS) -c $(MEMTIER_CONNECTIONS) -n $(MEMTIER_REQUESTS) -R $(MEMTIER_RAND) --ratio=$(MEMTIER_RATIO) -d $(MEMTIER_DATA_SIZE) > $(OUTPUT_FILE)

grep-stats: $(OUTPUT_FILE)
	@grep -A 10 "ALL STATS" $<

.PHONY: warmup expt all
