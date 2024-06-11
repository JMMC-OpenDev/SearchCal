#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************

#/**
# \file
# Test whether the environment (the current environment or the one passed as
# argument), is running or not.
#
# \synopsis
# \<envCheck\> [\e \<MCS_environment_name\>]
#
# \param MCS_environment_name : the MCS environment to check.
#
# \n
# \env
# MCSENV variable defines the current MCS environment name.
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
        # Over-ride the user MCSENV environment with the received one
        MCSENV=$1
    fi
fi

# If MCSENV is defined
if [ "$MCSENV" != "" ]
then
    # Set LABEL accordinaly
    LABEL="$MCSENV"
else
    # Set LABEL to "default"
    LABEL="default"
fi

# Check weither the msgManager is already running or not
TMP=`msgSendCommand msgManager PING "" 2>&1 > /dev/null`

# If the environment is not running
if [ "$?" != 0 ]
then
    echo "'$LABEL' environment is NOT running !"
else
    echo "'$LABEL' environment is running."
fi

exit 0;

#___oOo___
