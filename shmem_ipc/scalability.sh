#!/bin/bash

LOOP_COUNT=10	
CLIENTS=1       
CLIENTS_LIMIT=7  
ITERATIONS=100000

# Clean up and init 
echo "run,iterations,type,latency, client_id" >  results.csv


make clean > /dev/null
make > /dev/null

while [ $CLIENTS -le $CLIENTS_LIMIT ]
do
    i=0
    echo "---- ${CLIENTS} clients attached to server ---"
    while [ $i -lt $LOOP_COUNT ]
    do
      CUR_SUM=0
      taskset -c 0 ./server $ITERATIONS $CLIENTS > /dev/null &  
		  sleep 0.08
		  for (( c=1; c<=${CLIENTS}; c++ ))
		  do 
			  sleep 0.01
        taskset -c $c ./client $ITERATIONS 2>&1  &
		  done
      wait
      sleep 0.02
      i=$(( $i + 1 ))
    done
    CLIENTS=$(( $CLIENTS + 1 ))
done
