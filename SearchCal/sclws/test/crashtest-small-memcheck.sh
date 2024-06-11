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

VG_LOG=./vg_crash_memcheck.log
rm $VG_LOG
touch $VG_LOG

export VG_PATH=/home/users/bourgesl/apps/valgrind/bin

# valgrind memcheck options:  --show-reachable=yes --track-origins=yes
# --gen-suppressions=all --suppressions=./custom_suppressions.txt
$VG_PATH/valgrind -v --num-callers=40 --freelist-vol=500000000  --suppressions=./custom_suppressions.txt --leak-check=full --log-file=$VG_LOG $SCLWS_CMD &

# Remember server PID for later kill
PID=$!
echo "server started: $PID"

# Wait for server bind
sleep 9

for i in $(seq 2)
do
echo "iteration $i"
# queries (N, V, ...):
./testCrash.sh 4 &
./testCrash.sh 3 &
./testCrash.sh 2 &
./testCrash.sh 1
sleep 3
done

echo "waiting for processes (60s) ..."
# Wait for server overhead
sleep 60 

top -u bourgesl -b -n 1 > top-crashtest-small.log

echo "tests done. please stop server properly : kill $PID"
echo "Output: $LOG"

# kill server to get valgrind report
#echo -n "server stopping ..."
kill $PID

echo "done."

