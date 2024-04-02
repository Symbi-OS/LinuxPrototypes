CC = gcc
CFLAGS = -Wall -Wextra -fPIC -O2
LDFLAGS = -shared

TARGET = interpose.so

.PHONY: all clean

all: $(TARGET)

$(TARGET): interpose.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

# Count is 1 left shift 22
# COUNT = 4194304
# COUNT = 2097152
# Looks like all of these get abt the same xput
COUNT = 1048576
SIZE = 1
LOOPS = 30
# Taskset to core 4
TASKSET = taskset -c 4

run_interposed: ../write_loop $(TARGET)
	LD_PRELOAD=./$(TARGET) $(TASKSET) ../write_loop $(SIZE) $(COUNT) /tmp/test

run: ../write_loop $(TARGET) 
	$(TASKSET) ../write_loop $(SIZE) $(COUNT) /tmp/test

THROUGHPUT_PIPELINE := grep "Throughput" | cut -d ":" -f 2 | cut -d "M" -f 1 | tr -d " \t"

run_loop_xput: ../write_loop $(TARGET)
	for i in {1..$(LOOPS)}; do \
		$(TASKSET) ../write_loop $(SIZE) $(COUNT) /tmp/test 2>&1 | \
		$(THROUGHPUT_PIPELINE); \
	done

run_loop_xput_interposed: ../write_loop $(TARGET)
	for i in {1..$(LOOPS)}; do \
		LD_PRELOAD=./$(TARGET) $(TASKSET) ../write_loop $(SIZE) $(COUNT) /tmp/test 2>&1 | \
		$(THROUGHPUT_PIPELINE); \
	done

clean:
	rm -f $(TARGET)