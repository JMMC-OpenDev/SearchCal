#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_killcurlmemcheck.log

rm $VG_LOG
touch $VG_LOG

$VG_PATH/valgrind --run-libc-freeres=no -v --num-callers=12 --freelist-vol=500000000 --suppressions=./custom_suppressions.txt --log-file=$VG_LOG --leak-check=full --show-reachable=yes $SCLWS_CMD &> killcurlmemcheck.log &

# Remember server PID for later kill
VG_PID=$!
echo "valgrind started: $VG_PID"

# Wait for server bind
sleep 9

# BIG query in background:
./testPerfs.sh 1 &
sleep 1
./testPerfs.sh 1 &

# Wait a little before killing all cups instances (doing CDS Failures)
sleep 5 

killall -s SIGKILL -u bourgesl curl


# Wait a little before killing valgrind
sleep 3 

# kill server to get valgrind report
echo -n "valgrind stopping while request in progress ..."
kill -TERM $VG_PID 
echo "done."

# wait for valgrind shutdown hook ...
while [ $(ps -u bourgesl | grep $VG_PID | wc -l) -eq 1 ]
do
  sleep 1 
done

# Remove PID from output log
sed -i "s/^==[0123456789]*==//g" $VG_LOG
sed -i "s/^--[0123456789]*--//g" $VG_LOG
echo "Output: $VG_LOG"
