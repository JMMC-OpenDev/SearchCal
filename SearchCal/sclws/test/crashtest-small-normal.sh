#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

LOG=./crashtest-small.log

rm $LOG
touch $LOG

# test several allocators to release memory:
export G_SLICE=always-malloc
export GLIBCXX_FORCE_NEW=1

$SCLWS_CMD &> $LOG & 

# Remember server PID for later kill
PID=$!
echo "server started: $PID"

# Wait for server bind
sleep 9

for i in $(seq 10)
do
echo "iteration $i"
sleep 9 
# queries (N, V, ...):
./testCrash.sh 5 &
./testCrash.sh 4 &
./testCrash.sh 3 &
./testCrash.sh 2 &
./testCrash.sh 1
done

echo "waiting for processes (60s) ..."
# Wait for server overhead
sleep 60 

top -u bourgesl -b -n 1 > top-crashtest-small.log

echo "tests done. please stop server properly : kill $PID"
echo "Output: $LOG"

echo "done."

