#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

sclwsServer -v $VERBOSITY > sclwsServer.log &

# Remember server PID for later kill
VG_PID=$!
echo "server started: $VG_PID"

# Wait for server bind
sleep 9

# queries (N, V, ...):
./testBright.sh 1 &

# Wait few seconds before kill
sleep 3

# kill server to get valgrind report
echo -n "server stopping ..."
kill $VG_PID 
echo "done."
