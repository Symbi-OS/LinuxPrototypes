nop: nop.c
	$(CC) -o $@ $<

run: nop
	./nop

run_pt: nop
	shortcut.sh -p --- ./nop

run_interpose: nop
	shortcut.sh --- ./nop

run_be: nop
	shortcut.sh -be --- ./nop

clean:
	rm -f nop