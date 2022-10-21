#!/bin/bash

LOOP_COUNT=10

i=0
while [ $i -lt $LOOP_COUNT ]
do
	./server &> /dev/null &
	server_pid=$!

	sleep 0.5
	./client	

	wait
	i=$(( $i + 1 ))
done

