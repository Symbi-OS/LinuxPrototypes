CXX=g++
CXXFLAGS= -O3 -Wall -Wextra -std=c++17

SYMLIB_DIR=../../../Symlib
SYMLIB_LINK=-lSym
SYMLIB_INCLUDE=$(SYMLIB_DIR)/include
SYMLIB_LIB_DIR=$(SYMLIB_DIR)/dynam_build

all: independent_client independent_client_elevated client server server_elevated

independent_client: client.cpp
	$(CXX) $(CXXFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@ -D INDEPENDENT_CLIENT

independent_client_elevated: client.cpp
	$(CXX) $(CXXFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D INDEPENDENT_CLIENT -D ELEVATED

client: client.cpp
	$(CXX) $(CXXFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@

server: server.cpp
	$(CXX) $(CXXFLAGS) -I $(SYMLIB_INCLUDE) $^ -o $@

server_elevated: server.cpp
	$(CXX) $(CXXFLAGS) -I $(SYMLIB_INCLUDE) -L $(SYMLIB_LIB_DIR) $^ -o $@ $(SYMLIB_LINK) -D ELEVATED

clean:
	rm -rf independent_client independent_client_elevated client server server_elevated run_log



