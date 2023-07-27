all: default_linker_script.ld hi_ld_directive hi_ld_script check_same

hi_ld_directive: hi.c
	gcc $< -Xlinker --defsym=page_offset_base=0xffffffff826f1a60 -o $@

hi_ld_script: hi.c my_linker_script.ld
	gcc hi.c -T my_linker_script.ld -o $@

default_linker_script.ld:
	ld --verbose | awk 'BEGIN {p=0}; /^=======/ {if (p==0) p=1; else p=2; next}; p==1' > $@

check_same:
	md5sum hi_ld_directive
	md5sum hi_ld_script
clean:
	rm -rf default_linker_script.ld hi_ld*

