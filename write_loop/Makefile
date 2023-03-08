# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4 -g -fno-omit-frame-pointer -mno-red-zone

.PHONY: all clean run

EXEC=write_loop

# This is 1<<24
ITER?=$$(( 1 << 24))

FPATH?=/tmp/test

# How many bytes to write each iteration
WR_SZ?=1

SC_SCRIPT=shortcut.sh

APP_AND_ARGS=./$(EXEC) $(WR_SZ) $(ITER) $(FPATH)

write_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

all: run_elev_wr_only run_elev run_lower_wr_only run_elev_sc_wr_only

# Normal run
run: $(EXEC)
	$(APP_AND_ARGS)

# Should be identical to the normal run
run_passthrough: $(EXEC)
	$(SC_SCRIPT) -v -p --- $(APP_AND_ARGS)

# Interposing lib used this time, but no elevation or shortcutting
run_interposed: $(EXEC)
	$(SC_SCRIPT) -v --- $(APP_AND_ARGS)

# Whole application runs elevated, no shortcutting
run_elev: $(EXEC)
	$(SC_SCRIPT) -be --- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated, shortcutting write
run_elev_sc_wr_only: $(EXEC)
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- $(APP_AND_ARGS)

profile: $(EXEC)
	../profile.sh "'shortcut.sh -p --- $(APP_AND_ARGS)'" write_loop.svg

profile_interposed: $(EXEC)
	../profile.sh "'shortcut.sh --- $(APP_AND_ARGS)'" write_loop_interposed.svg

profile_elev: $(EXEC)
	../profile.sh "'shortcut.sh -be --- $(APP_AND_ARGS)'" write_loop_elev.svg

profile_sc: $(EXEC)
	../profile.sh "'shortcut.sh -be -s 'write->__x64_sys_write' --- $(APP_AND_ARGS)'" write_loop_sc.svg

profile_all:
	make profile
	make profile_interposed
	make profile_elev
	make profile_sc

clean:
	rm -rf results $(EXEC) test *.svg

# For loop that writes to a file 10 times
RESULT_RUNS=30
RESULT_ITER=$$(( 1 << 21 ))

baseline_ex: write_loop
	rm -f out.test
	for _ in $$(seq 1 $(RESULT_RUNS)); do make run ITER=$(RESULT_ITER) FPATH=/tmp/test &>> out.test; done
	mkdir -p results
	cat out.test  | grep Throughput | cut -d ' ' -f 2 > results/$@
	rm -f out.test

interposed_ex: write_loop
	rm -f out.test
	for _ in $$(seq 1 $(RESULT_RUNS)); do make run_interposed ITER=$(RESULT_ITER) FPATH=/tmp/test &>> out.test; done
	mkdir -p results
	cat out.test  | grep Throughput | cut -d ' ' -f 2 > results/$@
	rm -f out.test

elev_ex: write_loop
	rm -f out.test
	for _ in $$(seq 1 $(RESULT_RUNS)); do make run_elev ITER=$(RESULT_ITER) FPATH=/tmp/test &>> out.test; done
	mkdir -p results
	cat out.test  | grep Throughput | cut -d ' ' -f 2 > results/$@
	rm -f out.test

sc_ex: write_loop
	rm -f out.test
	for _ in $$(seq 1 $(RESULT_RUNS)); do make run_elev_sc_wr_only ITER=$(RESULT_ITER) FPATH=/tmp/test &>> out.test; done
	mkdir -p results
	cat out.test  | grep Throughput | cut -d ' ' -f 2 > results/$@
	rm -f out.test

run_ex: write_loop
	make baseline_ex
	make interposed_ex
	make elev_ex
	make sc_ex
PERF_CMD=perf stat -d -d -d
# PERF_CMD+=-e cycles,instructions,cache-references,cache-misses,bus-cycles
# PERF_CMD+=-e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores
PERF_CMD+=-e LLC-loads,LLC-load-misses,LLC-stores,LLC-prefetches

perf_stat:
	$(PERF_CMD) ./write_loop 1 $$(( 1 << 23 )) /tmp/test

sudo_decomp:
	sudo LD_LIBRARY_PATH=/home/sym/Symbi-OS/Symlib/dynam_build:/home/sym/Symbi-OS/Tools/bin/shortcut/deep_sc  BEGIN_ELE=1 SHORTCUT_write_TO___x64_sys_write=1 LD_PRELOAD=/home/sym/Symbi-OS/Tools/bin/shortcut/sc_lib.so  ./write_loop 1 $$((1<<22)) /tmp/test
# Examples:
# make profile ITER=$(( 1 << 25 )) FPATH=/tmp/test

# Depricate:

# run_elev_sc_wr_only_long: $(EXEC)
# 	$(SC_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 1073741824

# run_quick_sc: $(EXEC)
# 	$(SC_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 2

# run_elev_long: $(EXEC)
# 	$(SC_SCRIPT) -be --- ./$(EXEC) $(WR_SZ) 1073741824
# Uses Interposing lib, starts lowered and elevates on write
# run_elev_wr_only: $(EXEC)
#	$(SC_SCRIPT) -v -ewrite --- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated then lowers around write
# run_lower_wr_only: $(EXEC)
#	$(SC_SCRIPT) -v -be -lwrite --- $(APP_AND_ARGS)
