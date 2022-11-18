#!/bin/bash

LOOP_COUNT=6
ITERATIONS=100000          # 100k
ITERATION_LIMIT=1000000    # 1mil
ITERATION_INCREMENT=100000 # 100k
cur_time=$(date +%m-%d-%H-%M-%S)
EXPE_DIR="benchmark-${cur_time}"
EXPE_INFO="expe.info"

# Clean up and init 
echo "run,iterations,type,latency" > results.csv

mkdir $EXPE_DIR

echo "number of repeat run per iteration: ${LOOP_COUNT}" > $EXPE_INFO
echo "number of test iterations range: ${ITERATIONS} to ${ITERATION_LIMIT}" >> $EXPE_INFO
echo "number of each iterations increment: ${ITERATION_INCREMENT}" >> $EXPE_INFO

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

      taskset -c 0 ./server $ITERATIONS > /dev/null &
      sleep 0.08
      SC=$(taskset -c 1 ./client $ITERATIONS 2>&1)
      wait
      echo -n -e "  [===>......] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
      sleep 0.06

      # Check kernel version
      if [[ $(uname -r) == "5.14.0-symbiote+" ]];
      then
        echo -n -e "  [======>...] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
        ICE=$(taskset -c 1 bash -c 'shortcut.sh -be -- ./independent_client '$ITERATIONS' 2>&1 >/dev/null')
        sleep 0.06

        echo -n -e "  [======>...] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
        ICES=$(taskset -c 1 bash -c 'shortcut.sh -be -s "write->ksys_write" -- ./independent_client '$ITERATIONS' 2>&1 >/dev/null')
        sleep 0.06

        taskset -c 0 bash -c 'shortcut.sh -be -- ./server '$ITERATIONS' &> /dev/null' &
        sleep 0.08
        SCE=$(taskset -c 1 ./client $ITERATIONS 2>&1)
        wait
        echo -n -e "  [=======>..] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
        sleep 0.06

        taskset -c 0 bash -c 'shortcut.sh -be -s "write->ksys_write" -- ./server '$ITERATIONS' &> /dev/null' &
        sleep 0.08
        SCES=$(taskset -c 1 ./client $ITERATIONS 2>&1)
        wait
        echo -n -e "  [=======>..] Completed ${ITERATIONS} Iterations: $i/$LOOP_COUNT \r"
        sleep 0.06
        
        echo $i,$ITERATIONS,Independent Client \(elevated\),$ICE>> results.csv
        echo $i,$ITERATIONS,Independent Client \(elevated + shortcutted\),$ICES>> results.csv
        echo $i,$ITERATIONS,Client + Server \(elevated\),$SCE>> results.csv
        echo $i,$ITERATIONS,Client + Server \(elevated + shortcutted\),$SCES>> results.csv
      fi 

      echo $i,$ITERATIONS,Independent Client,$IC>> results.csv
      echo $i,$ITERATIONS,Client + Server,$SC>> results.csv

      sleep 0.02
      i=$(( $i + 1 ))
    done

    ITERATIONS=$(( $ITERATIONS + $ITERATION_INCREMENT ))
done

make clean > /dev/null

cp results.csv ./${EXPE_DIR}/
mv sys_info ./${EXPE_DIR}/
mv $EXPE_INFO ./${EXPE_DIR}/

mv ${EXPE_DIR} ./data

printf "\r----------------- COMPLETED -----------------\n\n"
