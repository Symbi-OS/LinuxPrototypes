#!/bin/bash

LOOP_COUNT=10

i=0
while [ $i -lt $LOOP_COUNT ]
do
	./server &> /dev/null &
	server_pid=$!

	./client
	
	ret_val=$?
	if [ $ret_val -ne 0 ]; then
		kill $server_pid
		echo "rerunning the server..."
		continue
	fi

	wait $server_pid
	printf "\n"
	sleep 1
	i=$(( $i + 1 ))
done

