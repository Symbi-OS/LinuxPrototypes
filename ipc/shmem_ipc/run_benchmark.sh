#!/bin/bash

LOOP_COUNT=500
#ITERATIONS=200000
ITERATIONS=1

function LaunchApproach1 {
    taskset -c 1 ./independent_client $ITERATIONS >> approach1_log.txt
    sleep 0.05
}

function LaunchApproach2 {
    taskset -c 0 ./server_no_work $ITERATIONS &> /dev/null &

    sleep 0.05
    taskset -c 1 ./client_work $ITERATIONS >> $log_file

    wait
}

function LaunchApproach3 {
    taskset -c 0 ./server_work $ITERATIONS &> /dev/null &

    sleep 0.05
    taskset -c 1 ./client_no_work $ITERATIONS >> $log_file

    wait
}

function LaunchApproach4 {
    taskset -c 0 ./server_work_elevated $ITERATIONS &> /dev/null &

    sleep 0.05
    taskset -c 1 ./client_no_work $ITERATIONS >> $log_file

    wait
}

function RunApproach {
	log_file=approach$1_log.txt

	rm -rf $log_file
	
	printf "Approach $1:\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	if [ $1 == "1" ]; then
            LaunchApproach1
        elif [ $1 == "2" ]; then
            LaunchApproach2
        elif [ $1 == "3" ]; then
            LaunchApproach3
        elif [ $1 == "4" ]; then
            LaunchApproach4
        fi

        ITERATIONS=$(( $ITERATIONS + 500 ))

    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
	done
	printf "\n\n"
}

printf "Choose one of the four approaches to IPC to test\n"
printf "\t[1] Independent Client (work)\n"
printf "\t[2] Client (work) + Server (no work)\n"
printf "\t[3] Client (no work) + Server (work)\n"
printf "\t[4] Client (no work) + Elevated Server (work)\n"
printf "\t[0] All approaches sequentially\n"
printf "\n"

read -p "Select your choice: " test_choice

if [ $test_choice == "0" ]; then
	RunApproach 1
    RunApproach 2
    RunApproach 3
    RunApproach 4
else
    RunApproach $test_choice
fi