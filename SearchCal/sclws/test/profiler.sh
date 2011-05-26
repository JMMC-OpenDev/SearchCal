#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_profiler.log

rm $VG_LOG
touch $VG_LOG

# valgrind options: 
valgrind --tool=callgrind --callgrind-out-file=$VG_LOG sclwsServer -v 1

