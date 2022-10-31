LOOP_COUNT=10	
SERVER=1
ITERATIONS=200000
IF_ELE=$1
i=0
        while [ $i -lt $LOOP_COUNT ]
        do
            if [ $1 -eq 1 ]
            then 
                ./elevated_server &
            else
                ./server &
            fi
            sleep 0.02
	    taskset -c 1 ./client $ITERATIONS $SERVER 1
            sleep 0.02
	    taskset -c 2 ./client $ITERATIONS $SERVER 2
            sleep 0.02
            i=$(( $i + 1 ))
            sleep 0.6
	    ./serverkill
        done
        printf "\n"
	wc tmp.txt


