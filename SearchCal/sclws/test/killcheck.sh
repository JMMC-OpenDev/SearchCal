#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_killcheck.log

rm $VG_LOG
touch $VG_LOG

# valgrind helgrind options:  --show-reachable=yes --track-origins=yes
# --gen-suppressions=all  --conflict-cache-size=5000000
$VG_PATH/valgrind -v --num-callers=12 --suppressions=./custom_suppressions.txt --tool=helgrind --read-var-info=yes --log-file=$VG_LOG $SCLWS_CMD &> killcheck.log &

# Remember server PID for later kill
VG_PID=$!
echo "valgrind started: $VG_PID"

# Wait for server bind
sleep 9

# BIG query in background:
./testPerfs.sh 1 &
sleep 1
./testPerfs.sh 2 &
sleep 1
./testPerfs.sh 3 &
sleep 1
./testPerfs.sh 4 &
sleep 1
./testPerfs.sh 5 &

# Wait a little before killing valgrind
sleep 3 

# kill server to get valgrind report
echo -n "valgrind stopping while request in progress ..."
kill -TERM $VG_PID 
echo "done."

# test request after kill:
./testPerfs.sh 0 &


# wait for valgrind shutdown hook ...
while [ $(ps -u bourgesl | grep $VG_PID | wc -l) -eq 1 ]
do
  sleep 1 
done

# Remove PID from output log
sed -i "s/^==[0123456789]*==//g" $VG_LOG
sed -i "s/^--[0123456789]*--//g" $VG_LOG
echo "Output: $VG_LOG"
