#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: envStop.sh,v 1.3 2005-02-13 16:53:13 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# lafrasse  21-Jan-2005  Created
# lafrasse  25-Jan-2005  Added MCSENV label management (for the default MCSENV)
#
#*******************************************************************************

#/**
# \file
# Stop the msgManager process associated with currently defined environment, or
# the one passed in argument.
#
# \synopsis
# \<envStop\> [\e \<MCS_environment_name\>]
#
# \param MCS_environment_name : the MCS environment the script should work in
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

# If MCSENV is defined
if [ "$MCSENV" != "" ]
then
    # Set LABEL accordinaly
    LABEL="$MCSENV"
else
    # Set LABEL to "default"
    LABEL="default"
fi

# Try to stop the msgManager
TMP=`msgSendCommand msgManager EXIT "" 2>&1 > /dev/null`

# If the environment could not be reached
if [ "$?" != 0 ]
then
    echo "'$LABEL' environment ALREADY terminated !"
else
    echo "'$LABEL' environment terminated."
fi

exit 0;

#___oOo___
