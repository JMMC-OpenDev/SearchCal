#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_tsancheck.log

rm $VG_LOG
touch $VG_LOG

export VALGRIND_LIB="$TSAN_PATH/lib/valgrind"
export VALGRIND_LIB_INNER="$TSAN_PATH/lib/valgrind"

# tsan options:  --show-reachable=yes --track-origins=yes
# --demangle=no --gen-suppressions=all 
# --hybrid=yes 
$TSAN_PATH/bin/valgrind --trace-children=no --keep-history=2 --num-callers=12 --ignore=./tsan_ignore.txt --suppressions=./custom_suppressions.txt --tool=tsan --hybrid=yes --announce-threads --read-var-info=yes $SCLWS_CMD 2>$VG_LOG &

# Remember server PID for later kill
VG_PID=$!
echo "tsan started: $VG_PID"

# Wait for server bind
sleep 3 

# queries (N, V, ...):
./testBright.sh 2 &
./testBright.sh 1

# Wait for tsan overhead
sleep 15

# kill server to get tsan report
echo -n "tsan stopping ..."
kill $VG_PID 
echo "done."

# wait for tsan shutdown hook ...
sleep 5

# Remove PID from output log
sed -i "s/^==[0123456789]*==//g" $VG_LOG
sed -i "s/^--[0123456789]*--//g" $VG_LOG
echo "Output: $VG_LOG"
