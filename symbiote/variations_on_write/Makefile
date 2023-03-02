EXEC=writes

INCLUDE=-I../../../Symlib/include
LIB=-L ../../../Symlib/dynam_build -lSym

CFLAGS=-mno-red-zone 

CFLAGS+=-O0 -g # -static

CFLAGS+=-Wall -Wextra -Werror
CFLAGS+=-Wfatal-errors

CFLAGS+=-Wno-format-overflow
# CFLAGS+=-Wunused-function
# CFLAGS+=-Wno-unused-variable
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
