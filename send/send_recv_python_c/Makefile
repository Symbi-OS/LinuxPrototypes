CC = gcc
CFLAGS = -Wall -Wextra -O3

TARGET = socket_comm

all: time_both

$(TARGET): socket_comm.c
	$(CC) $(CFLAGS) -o $(TARGET) socket_comm.c

SZ=8
COUNT=2000000

time_both: time_c time_py
time_c: socket_comm 
	time ./socket_comm $(COUNT) $(SZ)

time_py:
	time python3 socket_comm.py $(COUNT) $(SZ)

clean:
	rm -f $(TARGET)