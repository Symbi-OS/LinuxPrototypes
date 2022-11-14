#!/bin/bash

LOOP_COUNT=50

function RunApproach1 {
	log_file=approach1_log.txt

	rm -rf $log_file
	
	printf "Approach 1: Independent client (Un-elevated)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 0 ./independent_client 200000 >> approach1_log.txt
		sleep 0.2

    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
	done
	printf "\n\n"
}


function RunApproach2 {
	log_file=approach2_log.txt

    rm -rf $log_file

	printf "Approach 2: Independent client (Elevated + Shortcutted)\n"

	i=0
	while [ $i -lt $LOOP_COUNT ]
	do
    	taskset -c 0 ./independent_client_elevated 200000 >> $log_file
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
		taskset -c 0 ./server 200000 &> /dev/null &
		server_pid=$!

		sleep 0.5
		taskset -c 1 ./client 200000 >> $log_file

		wait
		i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
		sleep 0.2
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
    	taskset -c 0 ./server_elevated 200000 &> /dev/null &
    	server_pid=$!

    	sleep 0.08
    	taskset -c 1 ./client 200000 >> $log_file

    	wait
    	i=$(( $i + 1 ))
		echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
		sleep 0.06
	done
	printf "\n"
}

function RunApproach5 {
    log_file=approach5_log.txt

    rm -rf $log_file

    printf "Approach 5: Benchmark NO IPC\n"

    i=0
    while [ $i -lt $LOOP_COUNT ]
    do
        taskset -c 1 ./benchmark_client 200000 >> $log_file

        i=$(( $i + 1 ))
        echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.06
    done
    printf "\n"
}

function RunApproach6 {
    log_file=approach6_log.txt

    rm -rf $log_file

    printf "Approach 6: Benchmark IPC (ping-pong)\n"

    i=0
    while [ $i -lt $LOOP_COUNT ]
    do
        taskset -c 0 ./benchmark_server_nops 200000 &> /dev/null &
        server_pid=$!

        sleep 0.08
        taskset -c 1 ./benchmark_client_ipc 200000 >> $log_file

        wait
        i=$(( $i + 1 ))
        echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.06
    done
    printf "\n"
}

function RunApproach7 {
    log_file=approach7_log.txt

    rm -rf $log_file

    printf "Approach 7: Benchmark IPC\n"

    i=0
    while [ $i -lt $LOOP_COUNT ]
    do
        taskset -c 0 ./benchmark_server 200000 &> /dev/null &
        server_pid=$!

        sleep 0.08
        taskset -c 1 ./benchmark_client_ipc_no_work 200000 >> $log_file

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
printf "\t[4] Client + elevated and shortcutted Server\n\n"
printf "\t[5] Independent Benchmark Client\n"
printf "\t[6] Benchmark client + ping pong server\n"
printf "\t[7] Benchmark client + work server\n"

read -p "Select your choice: " test_choice

if [ $test_choice == "1" ]; then
	RunApproach1
elif [ $test_choice == "2" ]; then
    RunApproach2
elif [ $test_choice == "3" ]; then
    RunApproach3
elif [ $test_choice == "4" ]; then
    RunApproach4
elif [ $test_choice == "5" ]; then
    RunApproach5
elif [ $test_choice == "6" ]; then
    RunApproach6
elif [ $test_choice == "7" ]; then
	RunApproach7
fi

