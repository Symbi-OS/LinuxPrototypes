#!/bin/bash

LOOP_COUNT=10

printf "Approach 1: Independent client (Un-elevated)\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./independent_client
	sleep 0.2

    i=$(( $i + 1 ))
done

printf "\n"

printf "Approach 2: Independent client (Elevated + Shortcutted)\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./independent_client_elevated
    sleep 0.2

    i=$(( $i + 1 ))
done

printf "\n"
printf "Approach 3: Client + Un-elevated Server\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
	taskset -c 0 ./server &> /dev/null &
	server_pid=$!

	sleep 0.5
	taskset -c 1 ./client	

	wait
	i=$(( $i + 1 ))
done

printf "\n"
printf "Approach 4: Client + Elevated + Shortcutted Server\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./server_elevated &> /dev/null &
    server_pid=$!

    sleep 0.5
    taskset -c 1 ./client

    wait
    i=$(( $i + 1 ))
done


