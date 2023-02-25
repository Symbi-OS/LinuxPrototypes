EXEC=get_current

all: $(EXEC)

INCLUDE=-I../../../Symlib/include
LIB=-L ../../../Symlib/dynam_build -lSym

$(EXEC): $(EXEC).c
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) -o $@ $< ../../oot_module/hello.o



run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC)