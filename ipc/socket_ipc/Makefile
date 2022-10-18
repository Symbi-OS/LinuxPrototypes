# Symbiote library paths
SYMLIB_DIR = ../../../Symlib
SYMLIB_BUILD = $(SYMLIB_DIR)/dynam_build
SYMLIB_INCLUDE = $(SYMLIB_DIR)/include
SYMLIB_LINK = -lSym

# Kallsymlib
KALLSYMLIB=../../../kallsymlib/libkallsym.a

# Dynamic library path
LIBRARY_PATH = $(SYMLIB_BUILD)
export LIBRARY_PATH

all: server client

server: netutils.c server.c
	gcc -I $(SYMLIB_INCLUDE) $(SYMLIB_LINK) $(KALLSYMLIB) $^ -o $@

client: netutils.c client.c
	gcc -I $(SYMLIB_INCLUDE) $(SYMLIB_LINK) $(KALLSYMLIB) $^ -o $@

clean:
	rm -rf server client
