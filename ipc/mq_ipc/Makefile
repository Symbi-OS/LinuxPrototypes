.SILENT: clean

all: app ele_process

app: app_process.c
	gcc app_process.c -o app

ele_process: elevated_process.c
	gcc elevated_process.c -o ele_process

clean:
	rm -rf *~
	rm -f app ele_process
