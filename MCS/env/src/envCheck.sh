#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: envCheck.sh,v 1.2 2005-01-26 11:02:14 lafrasse Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# lafrasse  21-Jan-2005  Created
# lafrasse  25-Jan-2005  Added MCSENV label management (for the default MCSENV)
#
#*******************************************************************************

#/**
# \file
# Test weither the msgManager process associated with the currently defined
# environment name (or the one passed in argument), is running or not.
#
# \synopsis
# \<envCheck\> [\e \<MCS_environment_name\>]
#
# \param MCS_environment_name : the MCS environment the script should work in.
#
# \n
# \env
# MCSENV variable is read to get the currently defined MCS environment name.
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
