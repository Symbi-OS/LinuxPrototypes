all: main

# This is what the programmer intened to do.
# Main dynamically links with expected.
# =================================
run_main:
	LD_LIBRARY_PATH=$(PWD) ./main
# debug version
# LD_LIBRARY_PATH=$(PWD) LD_DEBUG=libs ./main

main: libShared.so
	gcc main.c -L./ -lShared -o $@

libShared.so: shared.c asm.S
	gcc -shared -fPIC -o $@ $^

# =================================

clean:
	- rm -rf main *.so
