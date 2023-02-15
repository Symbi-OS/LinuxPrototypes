# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4 -g -fomit-frame-pointer

.PHONY: all clean run

# all: run run_passthrough run_interposed run_elev_wr_only run_elev run_lower_wr_only \
# 		run_elev_sc_wr_only run_quick_sc
EXEC=write_loop
# This is 1<<24
#ITERATIONS=16777216

ITERATIONS=3677721

NR_BYTES_TO_WRITE=1

write_loop: write_loop.c
	gcc $(CFLAGS) -o $@ $<

all: run_elev_wr_only run_elev run_lower_wr_only run_elev_sc_wr_only

# Normal run
run: $(EXEC)
	./$(EXEC) $(NR_BYTES_TO_WRITE) $(ITERATIONS)

SHORTCUT_SCRIPT=shortcut.sh
APP_AND_ARGS=./$(EXEC) $(NR_BYTES_TO_WRITE) $(ITERATIONS)

# Should be identical to the normal run
run_passthrough: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -p --- $(APP_AND_ARGS)

# Interposing lib used this time, but no elevation or shortcutting
run_interposed: $(EXEC)
	$(SHORTCUT_SCRIPT) -v --- $(APP_AND_ARGS)

# Uses Interposing lib, starts lowered and elevates on write
run_elev_wr_only: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -ewrite --- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated then lowers around write
run_lower_wr_only: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -be -lwrite --- $(APP_AND_ARGS)

# Whole application runs elevated, no shortcutting
run_elev: $(EXEC)
	$(SHORTCUT_SCRIPT) -be --- $(APP_AND_ARGS)

run_elev_long: $(EXEC)
	$(SHORTCUT_SCRIPT) -be --- ./$(EXEC) $(NR_BYTES_TO_WRITE) 1073741824

# Uses Interposing lib, starts elevated, shortcutting write
run_elev_sc_wr_only: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -be -s 'write->__x64_sys_write' --- $(APP_AND_ARGS)

run_elev_sc_wr_only_long: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 1073741824

run_quick_sc: $(EXEC)
	$(SHORTCUT_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 2

profile:
	../profile.sh "'shortcut.sh -p --- $(APP_AND_ARGS)'" wr_loop.svg

clean:
	rm -rf $(EXEC) test
