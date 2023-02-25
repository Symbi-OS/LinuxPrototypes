#!/bin/bash

# This is busted, I tried...
set -x -e

sudo echo about to start profiling

TASKSET_CMD="taskset -c 1 bash -c"

# There must be 2 command line arguments
# The first is the workload command
# The second is the name of the svg output 
if [ $# -ne 2 ]; then
    echo "Usage: $0 <workload command> <svg output>"
    exit 1
fi
WORKLOAD_CMD=$1
# Wrap WORKLOAD_CMD in single quotes for composition with taskset and bash
SVG_OUTPUT=$2

# Run Workload
eval "$TASKSET_CMD $WORKLOAD_CMD &"

# Give the workload time to start
sleep 0.1

PID=$(pgrep -n write_loop)
echo $PID

sudo /usr/share/bcc/tools/profile -p $PID -d -I -af -F 999 10 > profile.out
# sudo /usr/share/bcc/tools/profile --stack-storage-size=$((1<<16)) -d -I -af -F 99 10 > profile.out

~/FlameGraph/flamegraph.pl profile.out > $SVG_OUTPUT

rm profile.out

