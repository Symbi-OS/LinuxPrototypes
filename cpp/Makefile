CPPFLAGS= -Wall -Wextra -Werror 
write_loop: write_loop.cc
	g++ $(CPPFLAGS) -o $@ $<

BYTES= 1
TIMES= $$((1<<20))
FILE= /tmp/test

run: write_loop
	./write_loop $(BYTES) $(TIMES) $(FILE)

run_sc: write_loop
	shortcut.sh -v -be -s 'write->__x64_sys_write' --- ./write_loop $(BYTES) $(TIMES) $(FILE)

clean:
	rm -f write_loop