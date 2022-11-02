LOOP_COUNT=$2	
ITERATIONS=$1
SERVER=1
INDEPENDENT=0
IF_ELE=0


# Clean up and init 
cp template.csv results.csv

i=0

while [ $i -lt $LOOP_COUNT ]
    do
        echo -n -e '  [>.........] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        make clean
        make > /dev/null
        sleep 0.1
        echo -n -e '  [=>........] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        IC=$(taskset -c 0 ./client $ITERATIONS $INDEPENDENT)
      	sleep 0.1

        ./server &
        sleep 0.1
	      SC=$(taskset -c 1 ./client $ITERATIONS $SERVER)
        echo -n -e '  [==>.......] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        sleep 0.1

        ./elevated_server &
        sleep 0.1
	      SCE=$(taskset -c 1 ./client $ITERATIONS $SERVER)
        echo -n -e '  [===>......] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        sleep 0.1

        ./server &
        sleep 0.1
	      SC1=$(taskset -c 2 ./client $ITERATIONS $SERVER)
        echo -n -e '  [====>.....] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        SC2=$(taskset -c 3 ./client $ITERATIONS $SERVER)
        echo -n -e '  [=====>....] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        sleep 0.1


        echo -n -e '  [======>...] Completed Iterations: '"$i/$LOOP_COUNT"'\r'


        ./elevated_server &
        sleep 0.1
	      SCE1=$(taskset -c 2 ./client $ITERATIONS $SERVER)
        echo -n -e '  [=======>..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        SCE2=$(taskset -c 3 ./client $ITERATIONS $SERVER)
        echo -n -e '  [========>.] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        sleep 0.1

        echo -n -e '  [=========>] Completed Iterations: '"$i/$LOOP_COUNT"'\r'

        echo $i, $IC, independent, unelevated>> results.csv
        echo $i, $SC, server, unelevated >> results.csv
        echo $i, $SCE, server, elevated >> results.csv
        echo $i, $SC1, client-1, unelevated >> results.csv
        echo $i, $SC2, client-2, unelevated >> results.csv
        echo $i, $SCE1, client-1, elevated >> results.csv
        echo $i, $SCE2, client-2, elevated >> results.csv
        i=$(( $i + 1 ))
        sleep 0.1
    done
    echo -n -e '  [==========] Completed Iterations: '"$LOOP_COUNT/$LOOP_COUNT"'\r'
    printf "\n"
        
