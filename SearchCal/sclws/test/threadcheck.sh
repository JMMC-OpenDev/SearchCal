#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_threadcheck.log

rm $VG_LOG
touch $VG_LOG

# valgrind options:  --show-reachable=yes --track-origins=yes
valgrind --tool=helgrind --read-var-info=yes --log-file=$VG_LOG  sclwsServer -v 1

