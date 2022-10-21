#!/bin/bash

LOOP_COUNT=100

function RunApproach1 {
	printf "Approach 1: Independent client (Un-elevated)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 0 ./independent_client
		sleep 0.2

    	i=$(( $i + 1 ))
	done
	printf "\n"
}


function RunApproach2 {
	printf "Approach 2: Independent client (Elevated + Shortcutted)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 0 ./independent_client_elevated
    	sleep 0.2

    	i=$(( $i + 1 ))
	done
	printf "\n"
}


function RunApproach3 {
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
}


function RunApproach4 {
	printf "Approach 4: Client + Elevated and Shortcutted Server\n"

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
	printf "\n"
}

printf "Choose one of the four approaches to IPC to test\n"
printf "\t[1] Independent client (un-elevated)\n"
printf "\t[2] Independent client (elevated + shortcutted)\n"
printf "\t[3] Client + un-elevated Server\n"
printf "\t[4] Client + elevated and shortcutted Server\n\n"

read -p "Select your choice: " test_choice
read -p "Log file: " log_file

if [ $test_choice == "1" ]; then
	RunApproach1 > $log_file
elif [ $test_choice == "2" ]; then
    RunApproach2 > $log_file
elif [ $test_choice == "3" ]; then
    RunApproach3 > $log_file
elif [ $test_choice == "4" ]; then
    RunApproach4 > $log_file
fi

