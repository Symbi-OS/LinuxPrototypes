.PHONY: all clean

EXEC = pg_undefines pg pg_broken
# EXEC += pg_ld_script
SRC = pg.c

CFLAGS=-Wall -Wextra 

LIB=-L./ -lkallsyms 
FPIC=-fPIC

all: $(EXEC)

libkallsyms.so: my.ld empty.c
	gcc empty.c -shared -o $@ -T $<

empty.c:
	touch $@

pg_undefines: $(SRC) libkallsyms.so
	gcc $(CFLAGS) -o $@ $^ $(LIB) $(FPIC) -u tu_main

pg: $(SRC) libkallsyms.so
	gcc $(CFLAGS) -o $@ $^ $(LIB) $(FPIC)

pg_broken: $(SRC) libkallsyms.so
	gcc $(CFLAGS) -o $@ $^ $(LIB)

compare: pg pg_broken
	nm pg | grep tu_main
	nm pg_undefines | grep tu_main
	nm pg_broken | grep tu_main

fixup_path:
	echo export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(PWD)

clean:
	rm -rf $(EXEC) libkallsyms.so empty.c
