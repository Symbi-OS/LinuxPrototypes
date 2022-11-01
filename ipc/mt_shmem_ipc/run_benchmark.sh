LOOP_COUNT=$2	
ITERATIONS=$1
SERVER=1
INDEPENDENT=0
IF_ELE=0

# id 0 is the independent server
# id 1 is server-client in single thread
# id 2 is first client in multithread
# id 3 is second client in multithread

# Clean up and init 
make clean
make > /dev/null

cp template.csv results.csv

i=0
while [ $i -lt $LOOP_COUNT ]
    do
        echo -n -e '  [..........] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        IC=$(taskset -c 0 ./client $ITERATIONS $INDEPENDENT)
        sleep 0.1
        echo -n -e '  [**........] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
      	sleep 0.1

        ./server &
        echo -n -e '  [***.......] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
	      SC=$(taskset -c 1 ./client $ITERATIONS $SERVER)
        echo -n -e '  [****......] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        echo -n -e '  [*****.....] Completed Iterations: '"$i/$LOOP_COUNT"'\r'

        ./server &
        sleep 0.1
        echo -n -e '  [******....] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
	      SC1=$(taskset -c 2 ./client $ITERATIONS $SERVER)
        echo -n -e '  [*******...] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        SC2=$(taskset -c 3 ./client $ITERATIONS $SERVER)
        echo -n -e '  [********..] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
        ./serverkill
        echo -n -e '  [*********.] Completed Iterations: '"$i/$LOOP_COUNT"'\r'

        echo $i, $IC, $SC, $SC1, $SC2 >> results.csv
        i=$(( $i + 1 ))
        sleep 0.1
        echo -n -e '  [**********] Completed Iterations: '"$i/$LOOP_COUNT"'\r'
        sleep 0.1
    done
    printf "\n"
        
