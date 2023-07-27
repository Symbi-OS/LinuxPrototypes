all: libkallsyms.so

# Generate the shared lib.
libkallsyms.so: my_ld_script.ld empty.c
	gcc empty.c -shared -o libkallsyms.so -T my_ld_script.ld

empty.c:
	touch empty.c

# Generate our linker script which is the default PLUS the non fn symbols.
my_ld_script.ld: default_ld_script.ld
	./gen_my_ld_script.sh > $@

# Get the default linker script from ld.
default_ld_script.ld:
	ld --verbose | awk 'BEGIN {p=0}; /^=======/ {if (p==0) p=1; else p=2; next}; p==1' > $@

clean:
	rm -f libkallsyms.so kallfns.s default_ld_script.ld my_ld_script.ld empty.c

