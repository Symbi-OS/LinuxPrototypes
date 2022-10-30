LOOP_COUNT=10
ITERATIONS=0
INDEPENDENT=0
SERVER=1

function RunApproach1 {
        log_file=approach1_log.txt

        rm -rf $log_file

        printf "Approach 1: Independent client (Un-elevated)\n"

        i=0
        while [ $i -lt $LOOP_COUNT ]
        do
            taskset -c 0 ./client $ITERATIONS $INDEPENDENT 0 >> approach1_log.txt
            sleep 0.02

            i=$(( $i + 1 ))
            echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        done
        printf "\n\n"
}


function RunApproach2 {
        log_file=approach2_log.txt

        rm -rf $log_file

        printf "Approach 2: Server with one client attached\n"

        i=0

        while [ $i -lt $LOOP_COUNT ]
        do
            timeout 2s taskset -c 0 ./server &> /dev/null &
            sleep 0.02
            taskset -c 1 ./client $ITERATIONS $SERVER 0 >> $log_file
	    wait
	    i=$(( $i + 1 ))
            echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        done
        printf "\n"
}


function RunApproach3 {
        log_file=approach3_log.txt

        rm -rf $log_file

        printf "Approach 3: Server with two client attached\n"

        i=0
        while [ $i -lt $LOOP_COUNT ]
        do
            timeout 2s taskset -c 0 ./server &> /dev/null &
            sleep 0.02
            taskset -c 1 ./client $ITERATIONS $SERVER 1 >> $log_file &
            sleep 0.02
            taskset -c 2 ./client $ITERATIONS $SERVER 2 >> $log_file &
	    wait
            i=$(( $i + 1 ))
            echo -n -e '  Completed Iterations: '"$i/$LOOP_COUNT"'\r'
            sleep 0.06
        done
        printf "\n"
}

make clean
make

printf "Choose one of the four approaches to IPC to test\n"
printf "\t[1] Independent client (Un-elevated)\n"
printf "\t[2] Server with one client\n"
printf "\t[3] Server with two client\n"

read -p "Select your choice: " test_choice
read -p "Enter each run's iteration count: " ITERATIONS

if [ $test_choice == "1" ]; then
        RunApproach1
elif [ $test_choice == "2" ]; then
    RunApproach2
elif [ $test_choice == "3" ]; then
    RunApproach3
fi
