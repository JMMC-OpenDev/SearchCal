#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: envStart.sh,v 1.8 2006-03-31 14:33:27 gzins Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.7  2005/12/06 11:44:17  gzins
# Improved error handling
#
# Revision 1.6  2005/03/08 10:11:09  mella
# place into real background msgManger with nohup
#
# Revision 1.5  2005/02/28 14:25:00  lafrasse
# Reversed changelog order
#
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
