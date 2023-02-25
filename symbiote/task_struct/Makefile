INCLUDE=../../../linux/include ../../../linux/arch/x86/include ./include_hacks \
		#/usr/include/

INCLUDE:=$(addprefix -I,$(INCLUDE))
# Stop compile at first error
CFLAGS=-Wall -Werror -Wfatal-errors 
DEFINES=-D__KERNEL__

# No standard includes
# CFLAGS+=-nostdinc

task_struct: task_struct.c
	$(CC) $(DEFINES) $(CFLAGS) -o $@ $< $(INCLUDE)

isol_ts: isol_ts.c
	$(CC) $(DEFINES) $(CFLAGS) -o $@ $< $(INCLUDE)

clean:
	rm -f task_struct