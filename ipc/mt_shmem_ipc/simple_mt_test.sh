LOOP_COUNT=10	
SERVER=$1
ITERATIONS=200000
i=0
        while [ $i -lt $LOOP_COUNT ]
        do
            taskset -c 1 ./client $ITERATIONS $SERVER 1
            sleep 0.02
            i=$(( $i + 1 ))
            sleep 0.06
        done
        printf "\n"



