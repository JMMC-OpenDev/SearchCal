#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Start the environment (the current environment or the one passed as argument).
#
# \synopsis
# \<envStart\> [\e \<MCS_environment_name\>]
#
# \param MCS_environment_name : the MCS environment to start
#
# \n
# \env
# MCSENV variable defines the current MCS environment name.
#
# */

# If we got more than 1 argument
if [ "$#" -gt 1 ]
then
    # Display the script usage
    echo -e "Usage: $0 [environment name]" 
    exit 1
else
    # If we got an enviromnent name
    if [ "$#" == 1 ]
    then 
        # Over-ride the user MCSENV environment with the given one
        MCSENV=$1
    fi
fi

# If MCSENV is defined
if [ "$MCSENV" != "" ]
then
    # Set envName accordinaly
    envName="$MCSENV"
else
    # Set envName to "default"
    envName="default"
fi

# Check environment exist 
answer=`envGet $envName 2>&1 > /dev/null`
if [ "$?" == 1 ]
then
    echo "'$envName' environment does not exist!" 
    exit 1;
fi

# Check whether the msgManager is already running or not
answer=`msgSendCommand msgManager PING "" 2>&1 > /dev/null`

# If the environment is not running
if [ "$?" != 0 ]
then
    echo "Starting '$envName' environment ..."
    procList="msgManager"
    # Start processes
    for proc in ${procList}
    do
        nohup $proc 2>&1 > /dev/null &
        sleep 1
        stat=`ps -f -u $USER | grep $proc | grep -v grep | wc -l`
        if [ $stat == 0 ]
        then
            echo "   '$proc' failed" >&2
            echo ""
            exit 1
        else
            echo "   '$proc' started" 
        fi
    done
    echo "done."
else
    echo "'$envName' environment ALREADY started !"
fi

exit 0;

#___oOo___
