#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# @file
# Basic compare 'sclsvr' tests and ref results.
#
# @synopsis
# \<sclsvrTestCompareResults\>
#
# @details
# Test results should be in 'sclsvr/test/test/results'.
# Any numerical value above 1.0e-2 relative error will be reported.
# */

RELATIVE_ERROR="1.0e-2"
NUMERICAL_SEPARATORS="[\[:& (),<>']"
REFERENCE_FILES="ref/results/*.out"

for REF_FILE in $REFERENCE_FILES;
do
    FILE_NAME=`basename $REF_FILE .out`
    echo $FILE_NAME
    cat ref/results/$FILE_NAME.cmd
    echo
    grep "Complete: done" */results/$FILE_NAME.out
    # was required by previous outputs
    # grep "Complete(): done" */results/$FILE_NAME.out

    echo

done

#___oOo___
