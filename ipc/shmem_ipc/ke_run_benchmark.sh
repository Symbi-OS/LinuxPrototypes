LOOP_COUNT=$2	
ITERATIONS=$1
SERVER=0
INDEPENDENT=1
IF_ELE=0
SLEEP_TIME=1


# Clean up and init 
cp template.csv results.csv

i=0

make clean > /dev/null
make > /dev/null

while [ $i -lt $LOOP_COUNT ]
    do
      rm -f tmp.txt
      echo -n -e '  [=>........] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      IC=$(taskset -c 0 ./client $ITERATIONS $INDEPENDENT write)
      sleep 0.02

      #echo -n -e '  [===>......] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      #ICE=$(taskset -c 0 ./client_elevated $ITERATIONS $INDEPENDENT write)
      #sleep 0.02

      #./server_elevated 1 > /dev/null &
      #sleep 0.02
      #SCE=$(taskset -c 1 ./client $ITERATIONS $SERVER write)
      #echo -n -e '  [======>...] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      #sleep $SLEEP_TIME

      taskset -c 1 ./server 1 > /dev/null &
      sleep 0.02
      SC=$(taskset -c 2 ./client $ITERATIONS $SERVER write)
      echo -n -e '  [=======>..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      sleep $SLEEP_TIME

      taskset -c 3 ./old_server $ITERATIONS > /dev/null &
      sleep 0.02
      SCO=$(taskset -c 4 ./client $ITERATIONS $SERVER write)
      echo -n -e '  [=======>..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      sleep $SLEEP_TIME

      taskset -c 5 ./old_server_elevated $ITERATIONS > /dev/null &
      sleep 0.02
      SCOE=$(taskset -c 6 ./client $ITERATIONS $SERVER write)
      echo -n -e '  [=======>..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      sleep $SLEEP_TIME


      echo $i, $IC, independent, unelevated>> results.csv
      #echo $i, $ICE, independent, elevated>> results.csv
      echo $i, $SC, server, unelevated>> results.csv
      echo $i, $SCO, old_server, unelevated>> results.csv
      echo $i, $SCOE, old_server, elevated>> results.csv
      #echo $i, $SCE, server, elevated >> results.csv
      sleep 0.02
	    i=$(( $i + 1 ))
      echo -n -e '  [=======>..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
    done
    echo -n -e '  [==========] Completed Iterations: '"$LOOP_COUNT/$LOOP_COUNT"'\r'
    printf "\n"
        
