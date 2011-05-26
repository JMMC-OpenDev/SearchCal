#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
source env.sh

VG_LOG=./vg_memcheck.log

rm $VG_LOG
touch $VG_LOG

# valgrind memcheck options:  --show-reachable=yes --track-origins=yes
valgrind --log-file=$VG_LOG --leak-check=full sclwsServer -v 1

