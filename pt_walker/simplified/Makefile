.PHONY: all clean

EXEC = pg pg_broken
SRC = pg.c

all: $(EXEC)

pg: $(SRC)
	gcc -o $@ $^ -L./ -lkallsyms -Wall -Wextra -u page_offset_base

pg_broken: $(SRC)
	gcc -o $@ $^ -lkallsyms -L./ -lkallsyms -Wall -Wextra

compare: pg pg_broken
	nm pg | grep page_offset_base
	nm pg_broken | grep page_offset_base


clean:
	rm -rf $(EXEC)
