#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

./monitor.sh "$SCLWS_CMD" &> monitor.log &

LOG=./crashtest.log

rm $LOG
touch $LOG

#export G_SLICE=always-malloc
#export GLIBCXX_FORCE_NEW=1

$SCLWS_CMD &> $LOG & 

# Remember server PID for later kill
PID=$!
echo "server started: $PID"

# Wait for server bind
sleep 9

for i in $(seq 100)
do
echo "iteration $i"
sleep 9 
# queries (N, V, ...):
./testBright.sh 10 &
./testBright.sh 9 &
./testBright.sh 8 &
./testBright.sh 7 &
./testBright.sh 6 &
./testBright.sh 5 &
./testBright.sh 4 &
./testBright.sh 3 &
./testBright.sh 2 &
./testBright.sh 1
done

echo "waiting for processes (60s) ..."
# Wait for server overhead
sleep 60 

echo "tests done. please stop server properly : kill $PID"
echo "Output: $LOG"

echo "done."

