# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4

.PHONY: all clean run

all: run run_passthrough run_interposed run_elev_wr_only run_elev run_lower_wr_only \
		run_elev_sc_wr_only

# This is 1<<24
ITERATIONS=16777216
NR_BYTES_TO_WRITE=1

wr_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

# Normal run
run: wr_loop
	./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

SHORTCUT_SCRIPT=~/Symbi-OS/Tools/bin/shortcut/shortcut.sh
APP_AND_ARGS=./wr_loop $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Should be identical to the normal run
run_passthrough: wr_loop
	$(SHORTCUT_SCRIPT) -v -p -- $(APP_AND_ARGS)

# Interposing lib used this time, but no elevation or shortcutting
run_interposed:
	$(SHORTCUT_SCRIPT) -v -- $(APP_AND_ARGS)

# Uses Interposing lib, starts lowered and elevates on write
run_elev_wr_only:
	$(SHORTCUT_SCRIPT) -v -ewrite -- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated then lowers around write
run_lower_wr_only:
	$(SHORTCUT_SCRIPT) -v -be -lwrite -- $(APP_AND_ARGS)

# Whole application runs elevated, no shortcutting
run_elev:
	$(SHORTCUT_SCRIPT) -be -- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated, shortcutting write
run_elev_sc_wr_only:
	$(SHORTCUT_SCRIPT) -v -be -s 'write->ksys_write' -- $(APP_AND_ARGS)

clean:
	rm -rf wr_loop