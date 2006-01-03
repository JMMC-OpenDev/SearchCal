#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclsvrTestGetCal.sh,v 1.1 2006-01-03 16:16:54 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
#*******************************************************************************

#/**
# @file
# sclsvrTestGetCal - Launch non-regressions test for the GETCAL command.
#
# @synopsis
# sclsvrTestGetCal
# */

# Print usage 
function printUsage () {
    echo -e "Usage: sclinsInstall [-h]" 
    echo -e "\t-h\tprint this help."
    exit 1;
}

# Launch an msg-based process 
function relaunchProcess () {
    # Ping the process
    msgSendCommand $1 PING "" >> /dev/null 2>&1

    # If it is running, stop it
    if [ $? == 0 ]
    then 
        msgSendCommand $1 EXIT "" >> /dev/null 2>&1
    fi

    # Start process
    $1 >> /dev/null 2>&1 &

    # Wait until 10 seconds for process to be registered 
    for (( i=0; $i<10; i++)); 
    do  
        msgSendCommand $1 PING "" >> /dev/null 2>&1

        # If the process has registerd correctly
        if [ $? == 0 ]; 
        then 
            return 0
        fi;

        # Otherwise sleep for 1 second
        sleep 1
    done

    return 1
}



# Parse command-line parameters
while getopts "h" option
# Only 'h' option is expected.
do
  case $option in
    h ) # Help option
        printUsage ;;
    * ) # Unknown option
        printUsage ;;
    esac
done

# Check that all options have been parsed 
if [ $# -ge $OPTIND ]
then 
    printUsage
fi

echo -e "Please wait while non-regression tests run..."

# Launch user environment is not already done
envStart >> /dev/null 2>&1
if [ $? != 0 ]
then
    echo -e "ERROR: Could not start user environment" >&2
    exit 1
fi

# Launch sclsvr is not already done
relaunchProcess sclsvrServer
if [ $? != 0 ] 
then
    echo -e "ERROR: Could not start sclsvrServer" >&2
    exit 1
fi

# Launch non-regression test
cmdBatch sclsvrTestGetCal.cfg >> /dev/null 2>&1
if [ $? != 0 ]
then
    echo -e "ERROR: Could not start tests batch execution" >&2
    exit 1
fi


# Find any differences between reference results and current results
diff -r references/ results/
if [ $? != 0 ]
then
    echo -e "ERROR: Differences found between reference result and new ones" >&2
    exit 1
fi

echo -e "OK: Regression test passed"
exit 0

#___oOo___
