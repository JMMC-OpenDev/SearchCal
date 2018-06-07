#!/bin/sh
if [ "`uname`" = "Linux" ]; then enable -n echo; fi
#
# sclwsManger:      SearchCal WebService Server
#
# Version:      0.1
# processname:  sclwsServer
# description:  Starts and stops the SearchCal WebService Server \
#               at boot time and shutdown.
# chkconfig: 2345 60 60

#!/bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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

_log()
{
  echo "$*"
  logger -t "${processId}-${processSoapPort}" "$*"
}
_log_error()
{
  echo "$*"
  logger -s -t "${processId}-${processSoapPort}" "$*"
}


# uncomment next line to accept some cores
# ulimit -c unlimited

# Define some usefull constants program name and process id
prog="sclwsServer -v 2"
processId="sclwsServer"
processSoapPort="8079"
logFile=/var/log/${processId}.log
userName="sclws"
PID_FILE="/var/run/${processId}.pid"
# 1M for max coredump size
#DAEMON_COREFILE_LIMIT=1024
# unlimited coredump size
DAEMON_COREFILE_LIMIT=unlimited 
# make sure it doesn't core dump anywhere unless requested
corelimit="ulimit -S -c ${DAEMON_COREFILE_LIMIT:-0}"

# used to gracefully stop the process (one checkpoint is tested each second for a faster restart)
let WAITINGPAUSE=10

INFODEBUG="(pid="$( cat "${PID_FILE}")" soapPort=${processSoapPort} username=${userName} logfile=${logFile} corefilelimit=${DAEMON_COREFILE_LIMIT} waitingpause=${WAITINGPAUSE})"

#handle parameter
case "$1" in
  start)
    _log "Starting $prog :"
    _log " ${INFODEBUG}"
   
    if [ `pidofproc -p "{$PID_FILE}" "${processId}"` ]
    then
      _log_error "Sorry, $prog is already running"
      failure
    else
  
      mv $logFile ${logFile}.$(date +"%F_%T") 

      # start real process and keep pid trace
      su - ${userName} -c "$corelimit >/dev/null 2>&1 ; ${prog} &" &> ${logFile}            
      ps -ef |grep -e "^${userName}"|grep sclwsServer|awk '{print $2}' > "${PID_FILE}"
      if [ $? -eq 0 ]
      then
        success 
      else
        _log_error "service can't be started ($prog)"
        failure
      fi
    fi
    ;;

  stop)
    _log "Shutting down (in max ${WAITINGPAUSE}s) $prog :"
    # kill term + loop to finish as soon as possible the stop action (with maximum delay before -9 signal)
    
    SC_PID=$(cat "${PID_FILE}")
    kill $SC_PID
    while [ $WAITINGPAUSE -gt 0 -a $(ps -u ${userName} | grep $SC_PID | wc -l) -eq 1 ]
    do
    let WAITINGPAUSE-=1
    sleep 1 
    done
    
    if [ $WAITINGPAUSE -eq 0 ]
    then
        _log_error "timeout expired, killproc sent to PID=${PID_FILE}"
        killproc -p "${PID_FILE}" "${processId}"
    else
        _log "program shutdown gracefully"
    fi
    ;;

  status)
    status -p "${PID_FILE}" "${processId}"
    ;;

  restart)
    _log "ReStarting $prog :"
    $0 stop
    $0 start
    ;;

  verify)
    if curl localhost:${processSoapPort} 2>/dev/null |grep "method not implemented" &> /dev/null 
    then 
      _log "service ${processId} PRODUCTION is running properly ${INFODEBUG}"
    else
      _log_error "service ${processId} PRODUCTION verification failed, resting a new one"
      $0 stop
      $0 start
    fi
    ;;

  *)
    _log "Usage: %s {start|stop|status|restart|verify}\n" "$prog"
    exit 1
esac

exit 0
