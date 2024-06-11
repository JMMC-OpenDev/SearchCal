#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

export G_SLICE=always-malloc
export GLIBCXX_FORCE_NEW=1

VG_LOG=./vg_drdcheck.log

rm $VG_LOG
touch $VG_LOG

# valgrind helgrind options:  --show-reachable=yes --track-origins=yes
# --gen-suppressions=all  --conflict-cache-size=5000000
$VG_PATH/valgrind -v --num-callers=12 --suppressions=./custom_suppressions.txt --tool=drd --read-var-info=yes --log-file=$VG_LOG $SCLWS_CMD &> drdcheck.log &

# Remember server PID for later kill
VG_PID=$!
echo "valgrind started: $VG_PID"

# Wait for server bind
sleep 20 

# queries (N, V, ...):
./testBright.sh 2 &
./testBright.sh 1

# Wait for valgrind overhead
sleep 15

# kill server to get valgrind report
echo -n "valgrind stopping ..."
kill $VG_PID 
echo "done."

# wait for valgrind shutdown hook ...
sleep 5

# Remove PID from output log
sed -i "s/^==[0123456789]*==//g" $VG_LOG
sed -i "s/^--[0123456789]*--//g" $VG_LOG
echo "Output: $VG_LOG"
