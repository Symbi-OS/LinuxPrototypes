send: send.c
	gcc -o $@ $<

ITER=1000
MSG_SZ=1000
run: 
	./send $(MSG_SZ) $(ITER)

run_be:
	shortcut.sh -be --- ./send $(MSG_SZ) $(ITER)
clean:
	rm -rf send
