run:
	sudo rm -rf /mnt/test.fio
	taskset -c 1 /usr/bin/fio benchmark.fio
run_pt:
	sudo rm -rf /mnt/test.fio
	shortcut.sh -p \
		--- sudo /usr/bin/fio benchmark.fio
run_sc:
	sudo rm -rf /mnt/test.fio
	taskset -c 1 bash -c 'shortcut.sh -be \
		-s "pwrite64->__x64_sys_pwrite64" \
		--- /usr/bin/fio benchmark.fio'

# -s "pwritev->__x64_sys_pwritev"
# -s "pread64->__x64_sys_pread64"
profile:
	make clean
	./profile.sh "'shortcut.sh -p --- sudo /usr/bin/fio benchmark.fio'" fio.svg

clean:
	rm -f fio.svg
	sudo rm -rf /mnt/test.fio

