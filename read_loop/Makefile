# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4

.PHONY: all clean run


# This is 1<<24
ITERATIONS=16777216
NR_BYTES_TO_WRITE=1

APP=./rd_loop

$(APP): read_loop.c
	gcc $(CFLAGS) -o $@ $<

all: run run_passthrough run_interposed run_elev_wr_only run_elev run_lower_wr_only \
		run_elev_sc_wr_only run_quick_sc

# Normal run
run: $(APP)
	$(APP) $(NR_BYTES_TO_WRITE) $(ITERATIONS)

SHORTCUT_SCRIPT=~/Symbi-OS/Tools/bin/shortcut/shortcut.sh
APP_AND_ARGS=$(APP) $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Should be identical to the normal run
run_passthrough: $(APP)
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
	$(SHORTCUT_SCRIPT) -v -be -s 'read->ksys_read' -- $(APP_AND_ARGS)

run_quick_sc:
	$(SHORTCUT_SCRIPT) -v -be -s 'read->ksys_read' -- $(APP) 1 2

clean:
	rm -rf $(APP)