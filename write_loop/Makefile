#.PHONY= clean results/baseline_xput_ex run_xput

SHELL := /bin/bash

CFLAGS=-O4 -g -fno-omit-frame-pointer -mno-red-zone

EXEC=write_loop

SC_SCRIPT=shortcut.sh
help:
	@echo this is a tool for studying throughput, latency, and power
	@echo of a loop doing writes with and without shortcutting
	@echo the main target of interest is run_all_exs
	@echo this assumes you're ok with running system wide perf collections
	@echo if you need that, consider the target enable_user_perf

write_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

write_loop_latency: write_loop.c
	gcc -DLATENCY $(CFLAGS) -o $@ $< 

######### Run all experiments #########
run_all_exs: run_xput_exs run_lat_exs run_power_exs

######### PARAMETERS #########
# How many times to repeat each experiment
RESULT_RUNS=3

ITER?=$$(( 1 << 21))

FPATH?=/tmp/test

# How many bytes to write each iteration
WR_SZ?=1

ARGS=$(WR_SZ) $(ITER) $(FPATH)

######### Throughput experiments #########
run_xput_exs: results/baseline_xput_ex results/sc_xput_ex

results/baseline_xput_ex: results
	@echo "Running baseline xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_baseline | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

results/sc_xput_ex: results
	@echo "Running shortcut xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_sc | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

run_xput_baseline: write_loop
	./write_loop $(ARGS)

run_xput_sc: write_loop
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)

######### Latency experiments #########
run_lat_exs: results/baseline_lat_ex results/sc_lat_ex

results/baseline_lat_ex: results
	@echo "Running baseline latency experiment"
	$(MAKE) run_lat_baseline
	mv latencies.txt $@

results/sc_lat_ex: results
	@echo "Running shortcut latency experiment"
	$(MAKE) run_lat_sc
	mv latencies.txt $@

run_lat_baseline: write_loop_latency
	./write_loop_latency $(ARGS)

run_lat_sc: write_loop_latency
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop_latency $(ARGS)

######### Power experiments #########
run_power_exs: results/baseline_power_ex results/sc_power_ex
PERF=perf stat -e energy-cores -e energy-gpu -e energy-pkg -e energy-psys -e energy-ram 

results/baseline_power_ex: results
	@echo "Running baseline power experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_power_baseline |& grep 'Joules energy-cores' | tr -s ' ' | cut -d ' ' -f 2 | tee -a $@; done

results/sc_power_ex: results
	@echo "Running shortcut power experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_power_sc |& grep 'Joules energy-cores' | tr -s ' ' | cut -d ' ' -f 2 | tee -a $@; done

run_power_baseline: write_loop
	$(PERF) ./write_loop $(ARGS)

run_power_sc: write_loop
	$(PERF) bash -c "$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)"

results:
	mkdir -p results

#disable need for sudo with perf
enable_user_perf:
	sudo sh -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'

PERF_CMD=perf stat -d -d -d
# PERF_CMD+=-e cycles,instructions,cache-references,cache-misses,bus-cycles
# PERF_CMD+=-e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores
PERF_CMD+=-e LLC-loads,LLC-load-misses,LLC-stores,LLC-prefetches

profile: write_loop
	../profile.sh "'shortcut.sh -p --- ./write_loop $(ARGS)'" write_loop.svg

profile_sc: write_loop
	../profile.sh "'shortcut.sh -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)'" ./write_loop_sc.svg

profile_all:
	make profile
	make profile_sc

perf_stat:
	$(PERF_CMD) ./write_loop 1 $$(( 1 << 23 )) /tmp/test

clean:
	rm -rf results write_loop write_loop_latency test *.svg latencies.txt
