
wr_rd: wr_rd.c
	gcc -o wr_rd wr_rd.c
run: wr_rd
	./wr_rd

run_interpose:
	shortcut.sh --- ./wr_rd

clean:
	rm -f wr_rd
	rm -f test.txt