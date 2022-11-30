#!/bin/bash

LOOP_COUNT=1
NUM_SERVER_THREADS=1
CLIENTS=4
NUM_SERVER_LIMIT=$CLIENTS
ITERATIONS=40



# Clean up and init 
make clean > /dev/null

echo ${LOOP_COUNT} >  scalability.log
echo ${CLIENTS} >>  scalability.log
echo $CLIENTS_LIMIT >>  scalability.log
echo $ITERATIONS >>  scalability.log

make > /dev/null
make expe_info > /dev/null

i=0
echo "---- ${CLIENTS} independent clients ---"
while [ $i -lt $LOOP_COUNT ]
do
  for (( c=1; c<=${CLIENTS}; c++ ))
  do 
    taskset -c $c ./independent_client $ITERATIONS 2>> scalability.log &
  done
  wait
  i=$(( $i + 1 ))
done

while [ $NUM_SERVER_THREADS -le $NUM_SERVER_LIMIT ]
do
    i=0
    echo "---- ${CLIENTS} clients attached to ${NUM_SERVER_THREADS}-threaded server ---"
    while [ $i -lt $LOOP_COUNT ]
    do
      MAX_CPU=$(( $NUM_SERVER_THREADS + 5 ))
      /usr/bin/time -v ./server $NUM_SERVER_THREADS > /dev/null &  
		  sleep 0.08
		  for (( c=1; c<=${CLIENTS}; c++ ))
		  do 
        taskset -c $c bash -c "LD_PRELOAD=/home/${USER}/Symbi-OS/Tools/bin/ipc_shortcut/ipc_shortcut.so ./independent_client ${ITERATIONS}" 2>> scalability.log &
		  done
      sleep 2
      ./server_killer $NUM_SERVER_THREADS > /dev/null
      sleep 0.02
      i=$(( $i + 1 ))
    done
    NUM_SERVER_THREADS=$(( $NUM_SERVER_THREADS + 1 ))
done
