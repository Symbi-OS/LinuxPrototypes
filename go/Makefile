# Build the golang filer write_loop.go
# GOFLAGS= -linkmode=shared
write_loop: write_loop.go
	go build $(GOFLAGS) -o write_loop write_loop.go

# write_loop.o: write_loop.go
# 	go build $(GOFLAGS) -o write_loop.o -buildmode=c-archive write_loop.go

write_loop_gcc: write_loop.go
	gcc $< -o $@

# Run the write_loop program
BYTES=1
ITER= $$(( 1 << 21 ))
FILE=/tmp/test

ARGS= $(BYTES) $(ITER) $(FILE)
APP=./write_loop $(ARGS)

# For timing, not for correctness
TASKSET=taskset -c 0
TIME=time
run: write_loop
	$(TIME) $(TASKSET) $(APP)

passthrough:
	$(TIME) $(TASKSET) shortcut.sh -p --- $(APP)

be:
	$(TIME) $(TASKSET) shortcut.sh -be -v --- $(APP)
sc:
	$(TIME) $(TASKSET) shortcut.sh -be -v -be -s 'write->__x64_sys_write' --- $(APP)

clean:
	rm -f write_loop