CFLAGS += -mno-red-zone -fno-omit-frame-pointer -O0 -g

benchmark: benchmark.c
	$(CC) $(CFLAGS) -o $@ $<

ITERATIONS=100000
USE_GLIBC_FN=0
run_pt:
	taskset -c 1 shortcut.sh -p --- ./benchmark $(ITERATIONS) $(USE_GLIBC_FN)

run_interpose:
	taskset -c 1 shortcut.sh --- ./benchmark $(ITERATIONS) $(USE_GLIBC_FN)

run_be:
	taskset -c 1 shortcut.sh -be --- ./benchmark $(ITERATIONS) $(USE_GLIBC_FN)

run_sc:
	taskset -c 1 shortcut.sh  \
		-s 'getpid->__x64_sys_getpid' \
		-s 'getppid->__x64_sys_getppid' \
		-s 'write->__x64_sys_write' \
		-s 'read->__x64_sys_read' \
		-s 'mmap->__x64_sys_mmap' \
		-s 'munmap->__x64_sys_munmap' \
		-s 'poll->__x64_sys_poll' \
		-s 'select->__x64_sys_select' \
		-l 'fork' \
		-e 'getpid' \ 
		--- ./benchmark $(ITERATIONS) $(USE_GLIBC_FN)

# use bpf to profile the benchmark
profile: benchmark profile.sh
	./profile.sh
# Build an executable called benchmark that depends on benchmark.c, use variable for dependency
clean:
	rm -rf benchmark benchmark_profile.svg profile.err profile.txt