#!/bin/bash

LOOP_COUNT=50
ITERATIONS=0

function RunApproach1 {
	log_file=approach1_log.txt

	rm -rf $log_file
	
	printf "Approach 1: Independent client (Un-elevated)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 6 ./independent_client $ITERATIONS >> approach1_log.txt
		sleep 0.02

    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
	done
	printf "\n"
}


function RunApproach2 {
	log_file=approach2_log.txt

    rm -rf $log_file

	printf "Approach 2: Independent client (Elevated + Shortcutted)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 6 ./independent_client_elevated $ITERATIONS >> $log_file
		sleep 0.02

    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
	done
	printf "\n"
}


function RunApproach3 {
	log_file=approach3_log.txt

    rm -rf $log_file

	printf "Approach 3: Client + Un-elevated Server\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
		taskset -c 6 ./server $ITERATIONS &> /dev/null &
		server_pid=$!

		sleep 0.08
		taskset -c 7 ./client $ITERATIONS >> $log_file

		wait
		i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
		sleep 0.06
	done
	printf "\n"
}


function RunApproach4 {
	log_file=approach4_log.txt

    rm -rf $log_file

	printf "Approach 4: Client + Elevated and Shortcutted Server\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 6 ./server_elevated $ITERATIONS &> /dev/null &
    	server_pid=$!

    	sleep 0.08
    	taskset -c 7 ./client $ITERATIONS >> $log_file

    	wait
    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
		sleep 0.06
	done
	printf "\n"
}

printf "Choose one of the four approaches to IPC to test\n"
printf "\t[1] Independent client (un-elevated)\n"
printf "\t[2] Independent client (elevated + shortcutted)\n"
printf "\t[3] Client + un-elevated Server\n"
printf "\t[4] Client + elevated and shortcutted Server\n"
printf "\t[5] All of the above (sequentially)\n\n"

read -p "Select your choice: " test_choice
read -p "Enter each run's iteration count: " ITERATIONS

if [ $test_choice == "1" ]; then
	RunApproach1
elif [ $test_choice == "2" ]; then
    RunApproach2
elif [ $test_choice == "3" ]; then
    RunApproach3
elif [ $test_choice == "4" ]; then
    RunApproach4
elif [ $test_choice == "5" ]; then
	RunApproach1
	printf "\n"
	RunApproach2
	printf "\n"
	RunApproach3
	printf "\n"
	RunApproach4
fi
