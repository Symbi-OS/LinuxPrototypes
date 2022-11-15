CC=gcc
CFLAGS= -O3 -ggdb -Wall -Wextra -mno-red-zone -lpthread -lrt
export CC CFLAGS

all: server client independent_client

server: server.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

client: client.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@

independent_client: client.c
	$(CC) $(CFLAGS) $^ -o $@ -D INDEPENDENT_CLIENT

independent_client_elevated: client.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D INDEPENDENT_CLIENT -D ELEVATED_MODE

independent_client_shortcut: independent_client.c
	LD_LIBRARY_PATH=$(LD_LIBRARY_PATH):/home/sym/Symbi-OS/Tools/bin/shortcut/ LD_PRELOAD="sc_lib.so" $(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

benchmark_tests: benchmark_client benchmark_client_nops benchmark_client_ipc benchmark_client_ipc_nops benchmark_client_ipc_no_work benchmark_server benchmark_server_nops

benchmark_client: client_bench.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D DO_WORK_WRITE

benchmark_client_nops: client_bench.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

benchmark_client_ipc_nops: client_bench.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D IPC_COMM

benchmark_client_ipc: client_bench.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D DO_WORK_WRITE -D IPC_COMM

benchmark_client_ipc_no_work: client_bench.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D DO_NO_WORK -D IPC_COMM

benchmark_server: server_bench.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR) -D DO_WORK_WRITE

benchmark_server_nops: server_bench.c ipc.c
	$(CC) $(CFLAGS) $^ -o $@ $(SYMLIB_LINK) -I $(SYMLIB_INCLUDE_DIR)

clean:
	rm -rf server server_elevated client independent_client stress_test_log \
		independent_client_elevated benchmark_client* benchmark_server*
