#!/bin/sh
if [ "`uname`" = "Linux" ]; then enable -n echo; fi
#
# sclwsManger:      SearchCal WebService Server
#
# Version:      0.1
# processname:  sclwsServer
# description:	Starts and stops the SearchCal WebService Server \
#		at boot time and shutdown.
# chkconfig: 2345 60 60

#!/bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: sclwsManager.sh,v 1.2 2011-01-13 08:48:04 mella Exp $"
#
# History
# -------
# $Log: not supported by cvs2svn $
# Revision 1.1  2008/02/18 15:31:44  lafrasse
# Creation.
#
#*******************************************************************************

#/**
# @file 
# SearchCal WebService Server Service management script.
# 
# @synopsis
# sclwsInit {start|stop|status|restart}
#
# @details
# This script is used by the init system. You need to copy it under /etc/init.d
# and run chkconfig --add sclwsManger . You can change the name if you prefer.
#
# */


# Source function library.
. /etc/rc.d/init.d/functions

# uncomment next line to accept some cores
# ulimit -c unlimited

# Define some usefull program name and process id
prog="sclwsServer"
processId="sclwsServer"
PID_FILE="/var/run/${prog}.pid"

#handle parameter
case "$1" in
  start)
  	gprintf "Starting %s: " "$prog"
    echo
   
    if [ `pidofproc -p "{$PID_FILE}" "${processId}"` ]
       then
           gprintf "Sorry, %s is already running" "$prog"
           failure
       else
       # start real process
       su - sclws -c "$prog" &
       echo "$!" > "${PID_FILE}"
       if [ $? -eq 0 ]
           then
           	success 
           else
           	failure
           fi
       fi
    echo
	;;
  stop)
	gprintf "Shutting down %s: " "$prog"
	killproc -p "${PID_FILE}" "${processId}"
	echo
	;;
  status)
	status -p "${PID_FILE}" "${processId}"
	;;
  restart)
  	gprintf "Re" 
  	gprintf "Starting %s: " "$prog"
	echo
        $0 stop
	$0 start
	;;
  *)
	gprintf "*** Usage: %s {start|stop|status|restart}\n" "$prog"
	exit 1
esac

exit 0
