#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

export G_SLICE=always-malloc
export GLIBCXX_FORCE_NEW=1

VG_LOG=./vg_memcheck.log

rm $VG_LOG
touch $VG_LOG

# valgrind memcheck options:  --show-reachable=yes --track-origins=yes
# --gen-suppressions=all
# --suppressions=./custom_suppressions.txt
$VG_PATH/valgrind --run-libc-freeres=no -v --num-callers=12 --freelist-vol=500000000 --suppressions=./custom_suppressions.txt --log-file=$VG_LOG --leak-check=full --show-reachable=yes $SCLWS_CMD &> memcheck.log &

# Remember server PID for later kill
VG_PID=$!
echo "valgrind started: $VG_PID"

# Wait for server bind
sleep 30 

# queries (N, V, ...):
./testBright.sh 1

# Wait for valgrind overhead
sleep 3

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
