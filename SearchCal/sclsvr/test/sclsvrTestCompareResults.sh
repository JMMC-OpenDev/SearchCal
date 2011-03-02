#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestCompareResults.sh,v 1.1 2011-03-02 16:52:16 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

#/**
# @file
# Numerically compare 'sclsvr' test and ref results.
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
REFERENCE_FILES="ref/*.out"
TEST_RESULT_DIR="test/results"

for REF_FILE in $REFERENCE_FILES;
do
    FILE_NAME=`basename $REF_FILE .out`
    TEST_FILE=${TEST_RESULT_DIR}/${FILE_NAME}

    printf "Comparing %-20s ... " "'${FILE_NAME}' results"
    ndiff -relerr ${RELATIVE_ERROR} -separators "${NUMERICAL_SEPARATORS}" ${REF_FILE} ${TEST_FILE}.out 2>&1 > ${TEST_FILE}.diff
    if [ $? -ne 0 ]
    then
        echo "FAILED."
    else
        echo "DONE."
        rm -f ${TEST_FILE}.diff
    fi
done

#___oOo___
