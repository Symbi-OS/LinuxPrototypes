#!/bin/bash

LOOP_COUNT=10

printf "Approach 1: Client + Un-elevated Server\n"

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

printf "\n"
printf "Approach 2: Client + Elevated Server\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    ./server_elevated &> /dev/null &
    server_pid=$!

    sleep 0.5
    ./client

    wait
    i=$(( $i + 1 ))
done


