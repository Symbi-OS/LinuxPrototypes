all: libkallsyms.so

libkallsyms.so: kallsyms.s
	gcc -shared -o libkallsyms.so kallsyms.s

kallsyms.s: genkallsyms.sh /proc/kallsyms
	./genkallsyms.sh > kallsyms.s

clean:
	rm -f libkallsyms.so kallsyms.s

