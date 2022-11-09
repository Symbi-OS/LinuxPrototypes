# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4

.PHONY: all clean run

# This is 1<<24
ITERATIONS=16777216
NR_BYTES_TO_WRITE=1

wr_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

# Normal run
run: wr_loop
	./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Should be identical to the normal run
run_passthrough: wr_loop
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -v -p -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Interposing lib used this time, but no elevation or shortcutting
run_interposed:
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Uses Interposing lib, starts lowered and elevates on write
run_elev_wr_only:
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -v -ewrite -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Uses Interposing lib, starts elevated then lowers around write
run_lower_wr_only:
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -v -be -lwrite -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Whole application runs elevated, no shortcutting
run_elev:
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -be -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Uses Interposing lib, elevates then lowers around elevate and shortcuts
run_elev_sc_wr_only:
	~/Symbi-OS/Tools/bin/shortcut/shortcut.sh -v -be -s 'write->ksys_write' -- ./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)


run_shortcut:
	# NYI


clean:
	rm -rf wr_loop