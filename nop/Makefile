nop: nop.c
	$(CC) -mno-red-zone -o $@ $<

nop_busted: nop.c
	$(CC) -o $@ $<

run: nop
	./nop

run_pt: nop
	shortcut.sh -p --- ./nop

run_interpose: nop
	shortcut.sh --- ./nop

run_be_nots:
	shortcut.sh -be --- ./nop

run_be: nop
	taskset -c 0 shortcut.sh -be --- ./nop

run_be_busted: nop_busted
	taskset -c 0 shortcut.sh -be --- ./nop_busted

run_be_bash: nop
	taskset -c 0 bash -c 'shortcut.sh -be --- ./nop'
clean:
	rm -f nop