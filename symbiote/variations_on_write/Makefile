EXEC=writes

INCLUDE=-I../../../Symlib/include
LIB=-L ../../../Symlib/dynam_build -lSym

# Using frame pointer makes allows compilation at -O2
# Because the push/pop ins that mess up the stack.
CFLAGS=-fno-omit-frame-pointer
# don't know if necessary if we got disabling interrupts working
CFLAGS+=-mno-red-zone

CFLAGS+=-O2 -g # -static

CFLAGS+=-Wall -Wextra -Werror
CFLAGS+=-Wfatal-errors

CFLAGS+=-Wno-format-overflow
CFLAGS+=-Wunused

$(EXEC): $(EXEC).c
	gcc $(CFLAGS) $(INCLUDE) $(LIB) -o $@ $< 

FILE=./test
TEST_ID=3
ITER=$$((1<<0))
SIZE=1

# TS=taskset -c 
run: $(EXEC)
	./writes $(FILE) $(TEST_ID) $(ITER) $(SIZE)
od: $(EXEC)
	objdump -S $(EXEC) | less
rebuild:
	make clean
	make	
clean:
	rm -f $(EXEC)
