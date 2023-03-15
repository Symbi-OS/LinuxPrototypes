
main: main.go getcr3.o
	gccgo $^ -o $@
#	go build $^ -o $@

getcr3.o: getcr3.S
	as -o $@ $<

run: main
	./main

# only worked in a 1 core vm...
run_elevated:
	shortcut.sh -be --- ./main

clean:
	rm -f main getcr3.o
