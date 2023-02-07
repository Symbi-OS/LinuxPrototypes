run:
	sudo rm -rf /mnt/test.fio
	sudo fio benchmark.fio
run_pt:
	sudo rm -rf /mnt/test.fio
	shortcut.sh -p \
		--- sudo /usr/bin/fio benchmark.fio
run_sc:
	sudo rm -rf /mnt/test.fio
	shortcut.sh -be \
		-s "pread64->__x64_sys_pread64" \
		-s "pwrite64->__x64_sys_pwrite64" \
		--- sudo /usr/bin/fio benchmark.fio

profile:
	make clean
	./profile.sh "'shortcut.sh -p --- sudo /usr/bin/fio benchmark.fio'" fio.svg

clean:
	rm -f fio.svg
	sudo rm -rf /mnt/test.fio

