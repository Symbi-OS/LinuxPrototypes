EXEC=get_fp

all: $(EXEC)

INCLUDE=-I../../../Symlib/include
LIB=-L ../../../Symlib/dynam_build -lSym
$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) -o $@ $<

ARGS=test.txt
run: $(EXEC)
	./$(EXEC) $(ARGS)

clean:
	rm -f $(EXEC)