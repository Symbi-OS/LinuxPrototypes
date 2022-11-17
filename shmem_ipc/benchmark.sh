#!/bin/bash

LOOP_COUNT=10
ITERATIONS=100000          # 100k
ITERATION_LIMIT=2000000    # 2m
ITERATION_INCREMENT=200000 # 200k

# Clean up and init 
echo "run,iterations,type,latency" >  results.csv

make clean > /dev/null
make > /dev/null

while [ $ITERATIONS -le $ITERATION_LIMIT ]
do
    i=0
    while [ $i -lt $LOOP_COUNT ]
    do

      echo -n -e "  [=>........] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
      IC=$(taskset -c 1 ./independent_client $ITERATIONS 2>&1)
      sleep 0.06

      echo -n -e "  [===>......] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
      ICE=$(taskset -c 1 ./independent_client_elevated $ITERATIONS 2>&1)
      sleep 0.06

      taskset -c 0 ./server_elevated  $ITERATIONS &> /dev/null &
      sleep 0.08
      SCE=$(taskset -c 1 ./client $ITERATIONS 2>&1)
      wait
      echo -n -e "  [======>...] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
      sleep 0.06

      taskset -c 0 ./server $ITERATIONS > /dev/null &
      sleep 0.08
      SC=$(taskset -c 1 ./client $ITERATIONS 2>&1)
      wait
      echo -n -e "  [=======>..] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
      sleep 0.06


      echo $i,$ITERATIONS,Independent Client,$IC>> results.csv
      echo $i,$ITERATIONS,Independent Client \(elevated\),$ICE>> results.csv
      echo $i,$ITERATIONS,Client + Server,$SC>> results.csv
      echo $i,$ITERATIONS,Client + Server \(elevated\),$SCE>> results.csv
      sleep 0.02
      i=$(( $i + 1 ))
    done
   ITERATIONS=$(( $ITERATIONS + $ITERATION_INCREMENT ))
done
