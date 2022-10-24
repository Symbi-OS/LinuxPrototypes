#!/bin/bash

LOOP_COUNT=10

make clean
make

printf "Approach 1: Independent client (Un-elevated)\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./independent
	sleep 0.2

    i=$(( $i + 1 ))
done


printf "\n"

printf "Approach 2: Independent client (Elevated + Shortcutted)\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./independent_elevated
    sleep 0.2

    i=$(( $i + 1 ))
done

printf "\n"

printf "Approach 3: Client + Un-elevated Server\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
	taskset -c 0 ./server & server_pid=$!

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
    taskset -c 0 ./server_elevated  & server_pid=$! 

    sleep 0.5
    taskset -c 1 ./client

    wait
    i=$(( $i + 1 ))
done

