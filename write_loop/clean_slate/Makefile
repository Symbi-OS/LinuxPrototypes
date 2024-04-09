
.PHONY: all clean

help:
	@echo help menu

# Count is 1 left shift 22
# COUNT = 4194304
# COUNT = 2097152
# Looks like all of these get abt the same xput
# COUNT = 1048576
COUNT = 524288 
SIZE = 64
LOOPS=30
TASKSET = taskset -c 4
FILE = /tmp/test
TARGET = ../write_loop

SC.SH = shortcut.sh

$(TARGET): ../write_loop.c
	make -C ../ write_loop

run_interposed:  $(TARGET)
	$(SC.SH)  --- $(TASKSET) $(TARGET) $(SIZE) $(COUNT) $(FILE) 

run:  $(TARGET) 
	$(TASKSET) $(TARGET) $(SIZE) $(COUNT) $(FILE) 

THROUGHPUT_PIPELINE := grep "Throughput" | cut -d ":" -f 2 | cut -d "M" -f 1 | tr -d " \t"

run_loop_xput:  $(TARGET)
	@for i in {1..$(LOOPS)}; do \
		$(TASKSET) $(TARGET) $(SIZE) $(COUNT) $(FILE)  2>&1 | \
		$(THROUGHPUT_PIPELINE); \
	done

run_loop_xput_interposed:  $(TARGET)
	@for i in {1..$(LOOPS)}; do \
		$(SC.SH)  --- $(TASKSET) $(TARGET) $(SIZE) $(COUNT) $(FILE)  2>&1 | \
		$(THROUGHPUT_PIPELINE); \
	done

stats: $(TARGET)
	@make --no-print-directory run_loop_xput | tee >(datamash mean 1 pstdev 1)
stats_interposed: $(TARGET)
	@make --no-print-directory run_loop_xput_interposed | tee >(datamash mean 1 pstdev 1)
# no interpose

# 189.22919763333 2.6920502313551
# 188.92595026667 2.3080395111256
#186.8635662     3.2052282404908

# unroll 8: 
# 188.22165626667 2.8275867533428
# 186.9573519     2.689396230627

# unroll 4:
# 191.4824673     2.556338279787
# 190.37946866667 4.3010733639541
# 191.92660563333 3.6360461149217

# unroll 2:
# 191.8760517     3.2804850682363
# 192.917799      3.1979266629595

# unroll 1:
# 194.27856456667 3.4093099257986
# 193.8262318     3.1126173248537



# with interpose
# 184.9577157     2.5413625515141
# 186.74133356667 2.1163348831338
# 184.51579823333 2.9486152317562

record:
	perf record $(TARGET) $(SIZE) $(COUNT) $(FILE) 

record_interposed:
	perf record $(SC.SH)  --- $(TASKSET) $(TARGET) $(SIZE) $(COUNT) $(FILE)

report:
	perf report

# run_loop_xput | tee >(datamash mean 1 pstdev 1)

clean:
	rm -f $(TARGET)