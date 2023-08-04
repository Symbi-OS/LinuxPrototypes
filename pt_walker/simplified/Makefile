.PHONY: all clean

#EXEC = pg pg_broken
EXEC = pg
SRC = pg.c

CFLAGS=-Wall -Wextra 

LIB=-L./ -lkallsyms -fPIC -mcmodel=large
all: $(EXEC)

libkallsyms.so: kallsyms.c
	gcc -o $@ $^ -shared -fPIC

# pg: $(SRC)
# 	gcc -o $@ $^ $(LIB) -u page_offset_base
pg: $(SRC)
	gcc -o $@ $^

# pg_broken: $(SRC)
# 	gcc -o $@ $^ $(LIB) -Wall -Wextra

compare: pg pg_broken
	nm pg | grep page_offset_base
	nm pg_broken | grep page_offset_base

fixup_path:
	echo export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(PWD)


clean:
	rm -rf $(EXEC)
