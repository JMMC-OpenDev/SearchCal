#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: envCheck.sh,v 1.1 2005-01-21 15:39:39 lafrasse Exp $"
#
# who       when         what
# --------  -----------  -------------------------------------------------------
# lafrasse  21-Jan-2005  Created
#
#
#*******************************************************************************

#/**
# \file
# Test weither the msgManager process associated with currently defined
# environment or the one passed in argument, is running or not.
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
        # Over-ride the user MCSENV environment with the given one
        MCSENV=$1
    fi
fi

# Check weither the msgManager is already running or not
output=`msgSendCommand msgManager PING "" 2>&1 > /dev/null`

# If the environment is not running
if [ "$?" != 0 ]
then
    echo "'$MCSENV' environment NOT running !"
else
    echo "'$MCSENV' environment running."
fi

exit 0;

#___oOo___
