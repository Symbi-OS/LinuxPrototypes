GCC=gcc
CFLAGS=-Wall -g
.PHONY: all clean run

all: sym_owner borrower

# Requires owner.c
%: %.c
	$(GCC) $(CFLAGS) $< -o $@

run: all
	# Get sudo check off timed path
	sudo echo hi
	./sym_owner &
	sudo ./borrower

clean:
	rm -f sym_owner borrower *.txt
    