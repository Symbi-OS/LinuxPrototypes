#!/bin/bash

LOOP_COUNT=10

make clean
make independent

printf "Approach 1: Independent client (Un-elevated)\n"

i=0
while [ $i -lt $LOOP_COUNT ]
do
    taskset -c 0 ./independent
	sleep 0.2

    i=$(( $i + 1 ))
done

printf "\n"

make clean
make independent_elevated

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

make clean
ipcrm --all=msg
make client
make server

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
	ipcrm --all=msg
done


printf "\n"


make clean
ipcrm --all=msg
make client
make server_elevated

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
    ipcrm --all=msg
done

