LOOP_COUNT=10	
SERVER=1
ITERATIONS=50000
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
            taskset -c 0 ./client $ITERATIONS $SERVER
      	    sleep 0.02
            i=$(( $i + 1 ))
            sleep 0.06
            ./serverkill
        done
        printf "\n"
	wc tmp.txt


