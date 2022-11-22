#!/bin/bash

LOOP_COUNT=10	
NUM_SERVER_THREADS=1
CLIENTS=1       
CLIENTS_LIMIT=3
ITERATIONS=100000


# Clean up and init 
make clean > /dev/null

echo ${LOOP_COUNT} >  scalability.log
echo ${CLIENTS} >>  scalability.log
echo $CLIENTS_LIMIT >>  scalability.log
echo $ITERATIONS >>  scalability.log

make > /dev/null

while [ $CLIENTS -le $CLIENTS_LIMIT ]
do
    i=0
    echo "---- ${CLIENTS} independent clients ---"
    while [ $i -lt $LOOP_COUNT ]
    do
		  for (( c=1; c<=${CLIENTS}; c++ ))
		  do 
			  sleep 0.01
        taskset -c $c ./independent_client $ITERATIONS 2>> scalability.log &
		  done
      wait
      sleep 0.02
      i=$(( $i + 1 ))
    done

    i=0
    echo "---- ${CLIENTS} clients attached to server ---"
    while [ $i -lt $LOOP_COUNT ]
    do
      taskset -c 0 ./server $NUM_SERVER_THREADS > /dev/null &  
		  sleep 0.08
		  for (( c=1; c<=${CLIENTS}; c++ ))
		  do 
			  #sleep 0.01
        taskset -c $c taskset -c $c bash -c "LD_PRELOAD=/home/${USER}/Symbi-OS/Tools/bin/ipc_shortcut/ipc_shortcut.so ./independent_client ${ITERATIONS}" 2>> scalability.log & 2>> scalability.log  &
		  done
      sleep 2
      ./server_killer 1
      sleep 0.02
      i=$(( $i + 1 ))
    done
    CLIENTS=$(( $CLIENTS + 1 ))
done

