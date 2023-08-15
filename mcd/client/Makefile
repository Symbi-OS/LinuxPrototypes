# Common Variables
SERVER = 172.16.97.208
PORT = 18080 
PROTOCOL = memcache_binary
THREADS = 4
CONNECTIONS = 50
REQUESTS = 5000
RATE = 10000
RATIO = 0:1
DATA_SIZE = 64
OUTPUT_FILE = output.txt

MTB=~/memtier_benchmark/memtier_benchmark

# Targets
run: REQUESTS = 20000
run:
	$(MTB) -s $(SERVER) -p $(PORT) -P $(PROTOCOL) -t $(THREADS) -c $(CONNECTIONS) -n $(REQUESTS) -R $(RATE) --ratio=$(RATIO) -d $(DATA_SIZE) > $(OUTPUT_FILE)

grep-stats: run
	@grep -A 10 "ALL STATS" $(OUTPUT_FILE)

clean:
	rm -f $(OUTPUT_FILE)

.PHONY: run grep-stats
