# Interposing lib | Always elevated | Elevated on write | Shortcutting write

SHELL := /bin/bash

CFLAGS=-O4 -g -fomit-frame-pointer

.PHONY: all clean run

# all: run run_passthrough run_interposed run_elev_wr_only run_elev run_lower_wr_only \
# 		run_elev_sc_wr_only run_quick_sc
EXEC=write_loop

# This is 1<<24
ITER?=16777216

FPATH?=./test

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

# Uses Interposing lib, starts lowered and elevates on write
run_elev_wr_only: $(EXEC)
	$(SC_SCRIPT) -v -ewrite --- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated then lowers around write
run_lower_wr_only: $(EXEC)
	$(SC_SCRIPT) -v -be -lwrite --- $(APP_AND_ARGS)

# Whole application runs elevated, no shortcutting
run_elev: $(EXEC)
	$(SC_SCRIPT) -be --- $(APP_AND_ARGS)

# Uses Interposing lib, starts elevated, shortcutting write
run_elev_sc_wr_only: $(EXEC)
	$(SC_SCRIPT) -v -be -s 'write->__x64_sys_write' --- $(APP_AND_ARGS)

profile:
	../profile.sh "'shortcut.sh -p --- $(APP_AND_ARGS)'" wr_loop.svg

clean:
	rm -rf $(EXEC) test

# Depricate:

# run_elev_sc_wr_only_long: $(EXEC)
# 	$(SC_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 1073741824

# run_quick_sc: $(EXEC)
# 	$(SC_SCRIPT) -v -be -s 'write->ksys_write' --- ./$(EXEC) 1 2

# run_elev_long: $(EXEC)
# 	$(SC_SCRIPT) -be --- ./$(EXEC) $(WR_SZ) 1073741824