.SILENT: clean

all: sys_write_server client sys_write

sys_write_server: sys_write_server.c
	gcc sys_write_server.c -o sys_write_server

client: client.c
	gcc client.c -o client

sys_write: sys_write.c
	gcc sys_write.c -o sys_write

clean:
	rm -rf *~
	rm -f sys_write_server client sys_write tmp.txt
	touch tmp.txt

