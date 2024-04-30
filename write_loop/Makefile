#.PHONY= clean results/baseline_xput_ex run_xput

SHELL := /bin/bash

CFLAGS=-O4 -g -fno-omit-frame-pointer -mno-red-zone

EXEC=write_loop

SC_SCRIPT=shortcut.sh
help:
	@echo this is a tool for studying throughput, latency, and power
	@echo of a loop doing writes with and without shortcutting
	@echo the main target of interest is run_all_exs
	@echo this assumes youre ok with running system wide perf collections
	@echo if you need that, consider the target enable_user_perf

write_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

write_loop2: write_loop.c
	gcc $(CFLAGS) -I../../Symlib/include -L../../Symlib/dynam_build -lSym -o $@ $< sc_lib.o

write_loop_sc: write_loop.c
	gcc $(CFLAGS) -I../../Symlib/include -L../../Symlib/dynam_build -lSym -DSYM_SHORTCUT -o $@ $<

write_loop_latency: write_loop.c
	gcc -DLATENCY $(CFLAGS) -o $@ $< 

write_loop2_latency: write_loop.c
	gcc -DLATENCY $(CFLAGS) -I../../Symlib/include -L../../Symlib/dynam_build -lSym -o $@ $< sc_lib.o

write_loop_sc_latency: write_loop.c
	gcc -DLATENCY $(CFLAGS) -I../../Symlib/include -L../../Symlib/dynam_build -lSym -DSYM_SHORTCUT -o $@ $<

######### Run all experiments #########
run_all_exs: run_xput_exs run_lat_exs run_power_exs

######### PARAMETERS #########
# How many times to repeat each experiment
RESULT_RUNS=60

ITER?=$$(( 1 << 22))

FPATH?=/tmp/test

# How many bytes to write each iteration
WR_SZ?=1

ARGS=$(WR_SZ) $(ITER) $(FPATH)

######### Throughput experiments #########
run_xput_exs: results/baseline_xput_ex results/sc_xput_interpose_no_sc results/sc_xput_ex results/sc_xput_manual_sc

results/vanilla_baseline_xput_ex: results
	@echo "Running baseline xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_baseline | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done
	
	mkdir -p results/throughput
	mv $@ results/throughput/

results/baseline_xput_ex: results
	@echo "Running baseline xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_baseline | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done
	
	mkdir -p results/throughput
	mv $@ results/throughput/

results/sc_xput_interpose_no_sc: results
	@echo "Running shortcut xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_interpose_no_sc | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/throughput
	mv $@ results/throughput/

results/sc_xput_ex: results
	@echo "Running shortcut xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_sc | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/throughput
	mv $@ results/throughput/

results/sc_xput_static_sc_lib_link_sc: results
	@echo "Running shortcut xput static sc_lib link experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_static_sc_lib_link_sc | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/throughput
	mv $@ results/throughput/

results/sc_xput_manual_sc: results
	@echo "Running manual in-source shortcut xput experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_xput_manual_sc | grep Throughput | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/throughput
	mv $@ results/throughput/

run_xput_baseline: write_loop
	./write_loop $(ARGS)

run_xput_interpose_no_sc: write_loop
	$(SC_SCRIPT) -v --- ./write_loop $(ARGS)

run_xput_sc: write_loop
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)

run_xput_static_sc_lib_link_sc: write_loop2
	./write_loop2 $(ARGS)

run_xput_manual_sc: write_loop_sc
	./write_loop_sc $(ARGS)

######### Latency experiments #########
run_lat_exs: results/baseline_lat_ex results/interposed_lat_ex results/sc_lat_ex results/manual_sc_lat_ex

# Run run_lat_baseline 30x each time move the latencies.txt file to be named latencies_<throughput>
# where throughput is read out of the output. When done, sort the files by throughput and save
# the median file in results/baseline_lat_ex_median. Delete the rest.
# There might be an off by one error here, but I don't really care.
results/vanilla_baseline_lat_ex: results baseline_tmp_lat
	@echo "Running vanilla baseline latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_baseline | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt baseline_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls baseline_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

results/baseline_lat_ex: results baseline_tmp_lat
	@echo "Running baseline latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_baseline | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt baseline_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls baseline_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

results/interposed_lat_ex: results interposed_tmp_lat
	@echo "Running shortcut latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_interposed | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt interposed_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls interposed_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

results/sc_lat_ex: results sc_tmp_lat
	@echo "Running shortcut latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_sc | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt sc_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls sc_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

results/static_sc_lib_link_sc_lat_ex: results static_sc_lib_link_tmp_lat
	@echo "Running static sc_lib link shortcut latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_static_sc_lib_link_sc | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt static_sc_lib_link_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls static_sc_lib_link_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

results/manual_sc_lat_ex: results manual_sc_tmp_lat
	@echo "Running shortcut latency experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		XPUT=$$($(MAKE) run_lat_manual_sc | grep Throughput | cut -d ' ' -f 2 ) ; \
		mv latencies.txt manual_sc_tmp_lat/latencies_$$XPUT; \
	done
	MEDIAN_LAT=$$(ls manual_sc_tmp_lat/latencies_* | sort -n -k 2 | head -n $$(($(RESULT_RUNS)/2)) | tail -n 1 ); \
	cp $$MEDIAN_LAT $@;

	mkdir -p results/latency
	mv $@ results/latency/

# Someone clean up this midnight hacking
baseline_tmp_lat:
	mkdir -p $@
interposed_tmp_lat:
	mkdir -p $@
sc_tmp_lat:
	mkdir -p $@
static_sc_lib_link_tmp_lat:
	mkdir -p $@
manual_sc_tmp_lat:
	mkdir -p $@

run_lat_baseline: write_loop_latency
	./write_loop_latency $(ARGS)

run_lat_interposed: write_loop
	$(SC_SCRIPT) -v --- ./write_loop_latency $(ARGS)

run_lat_sc: write_loop_latency
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop_latency $(ARGS)

run_lat_static_sc_lib_link_sc: write_loop2_latency
	./write_loop2_latency $(ARGS)

run_lat_manual_sc: write_loop_sc_latency
	./write_loop_sc_latency $(ARGS)

######### Power experiments #########
run_power_exs: results/baseline_power_ex results/sc_power_ex
PERF=perf stat -e energy-cores -e energy-gpu -e energy-pkg -e energy-psys -e energy-ram 

results/vanilla_baseline_power_ex: results
	@echo "Running baseline power experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_power_baseline |& grep 'Joules energy-cores' | tr -s ' ' | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/power
	mv $@ results/power/

results/baseline_power_ex: results 
	@echo "Running baseline power experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_power_baseline |& grep 'Joules energy-cores' | tr -s ' ' | cut -d ' ' -f 2 | tee -a $@; done
	
	mkdir -p results/power
	mv $@ results/power/

results/sc_power_ex: results
	@echo "Running shortcut power experiment"
	for i in $$(seq 1 $(RESULT_RUNS)); do \
		echo run $$i; \
		$(MAKE) run_power_sc |& grep 'Joules energy-cores' | tr -s ' ' | cut -d ' ' -f 2 | tee -a $@; done

	mkdir -p results/power
	mv $@ results/power/

run_power_baseline: write_loop
	$(PERF) ./write_loop $(ARGS)

run_power_sc: write_loop
	$(PERF) bash -c "$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)"

results:
	mkdir -p results

#disable need for sudo with perf
enable_user_perf:
	sudo sh -c 'echo -1 > /proc/sys/kernel/perf_event_paranoid'

#PERF_CMD=perf stat -d -d -d
# PERF_CMD+=-e cycles,instructions,cache-references,cache-misses,bus-cycles
# PERF_CMD+=-e L1-dcache-loads,L1-dcache-load-misses,L1-dcache-stores
#PERF_CMD+=-e LLC-loads,LLC-load-misses,LLC-stores,LLC-prefetches

profile: write_loop
	../profile.sh "'shortcut.sh -p --- ./write_loop $(ARGS)'" write_loop.svg

profile_sc: write_loop
	../profile.sh "'shortcut.sh -be -s 'write->__x64_sys_write' --- ./write_loop $(ARGS)'" ./write_loop_sc.svg

profile_all:
	make profile
	make profile_sc

clean:
	rm -rf results write_loop write_loop_latency write_loop_sc test *.svg latencies.txt baseline_tmp_lat sc_tmp_lat
