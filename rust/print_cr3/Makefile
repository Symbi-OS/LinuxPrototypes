build:
	rustc -O main.rs
run:
	./main
clean:
	rm -f main

be:
	shortcut.sh -be --- ./main

install_rust:
	sudo dnf install rust cargo

objdump:
	objdump -S main | c++filt | less