#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: envStart.sh,v 1.5 2005-02-28 14:25:00 lafrasse Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.4  2005/02/13 17:26:51  gzins
# Minor changes in documentation
#
# Revision 1.3  2005/02/13 16:53:13  gzins
# Added CVS log as modification history
#
# lafrasse  25-Jan-2005  Added MCSENV label management (for the default MCSENV)
# lafrasse  21-Jan-2005  Created
#
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
    # Try to start the msgManager
    msgManager 2>&1 > /dev/null &
    echo "'$LABEL' environment started."
else
    echo "'$LABEL' environment ALREADY started !"
fi

exit 0;

#___oOo___
