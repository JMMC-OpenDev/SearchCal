/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: log.c,v 1.13 2004-06-22 07:02:09 gzins Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* mella     07-May-2004  created preliminary version based on log from VLT/ESO
* gzins     16-Jun-2004  removed logIdentify function; replaced by mcsInit and
*                        mcsGetProcName functions
*-----------------------------------------------------------------------------*/

/**
\mainpage Log : MCS C Logging module

\htmlonly
<center>
<a href="#summary">Summary</a>
<a href="#requirements">Requirements</a>
<a href="#installation">Installation</a>
<a href="#config">Configuration</a>
<a href="#copyright">Copyright</a>
</center>
\endhtmlonly

\section summary Summary

Log is a C module for logging to syslog or stdout. It is a module of the MCS
software.
Log is available from the CVS repository of JMMC or is included in the MCS
distribution. 

\section requirements Requirements
 
\subsection platforms Platforms

log module was successfully compiled and run on the following platforms :

\li Mandrake Linux release 9.2

log module should compile and run on the following platforms, but has never
been :

\li Other Linux distributions

\subsection software Software

The following softwares are needed to generate the log library:

\li MCS environment

This library generates log messages using syslog(), which will  be
distributed  by  syslogd. By default, log uses local3 facility.

The following softwares are needed to use the log library:

\li syslogd

\section installation Installation
 
\subsection installation_cvs Building from cvs's sources

The log module uses the MCS makefile system. The following commands should
build log on the supported platforms:

\code

$ cvs co log
$ cd log/src
$ make

\endcode

The following command should install the log library into the INTROOT
directory defined by the environment variable $INTROOT:

\code

$ make install

\endcode

\section config Configuration
This modules makes call to the syslogd daemon. syslogd must be configured to
route the messages to the correct destination.

\subsection conf_files Configuration files
In general, \p /etc/syslog.conf configures the sylogd. This file is taken
into account at startup or after a SIGHUP signal sent. \p /etc/init.d/syslogd
\p restart also makes \p syslogd reread configuration
file.  

\subsection conf_syntax Configuration syntax
Simplest configuration lines is:
\code

local3.*                            /home/MCS/mcs.log

\endcode

See syslog.conf manual page for further information.

\subsection env Environment variables

No use at the present time.

\section copyright Copyright

All software in this package are Copyright © 2004 JMMC
http://mariotti.ujf-grenoble.fr 

*/

/** \file
 * This module provides functions that enable users to handle the three types
 *   of logs for the Event Logging. The types of logs are:
 *   \li Standard Logs : These logs are stored into the standard MCS logger.
 *   \li Verbose Logs  : These logs are written to stdout.
 *   \li Action Logs   : TBD.
 *
 *   The three kinds of logs contain the same information, but are stored
 *   in different locations and the levels of information are controlled
 *   individually by logSetLogLevel(), logSetVerboseLevel() and
 *   logSetActionLevel() methods. The current log levels can be retrieved by
 *   logGetLogLevel(), logGetVerboseLevel() and logGetActionLevel() methods.
 *
 *   The logging levels range from \p logQUIET to \p logEXTDBG , where the
 *   lowest number means the  lowest priority. By default, the level of all
 *   the logs is set to \p logWARNING.
 *   The log and verbose flags indicate if the log information are presented
 *   respectively on the standard \p MCS logger or on the stdout device
 *   (default is mcsTRUE).
 *   The following enums should be used to set the log levels : \n
 *   \li \p logQUIET   (0) : no echo \n
 *   \li \p logWARNING (1) : errors or abnormal events for application.\n
 *   \li \p logINFO    (2) : major events
 *   (e.g when operational mode is modified).\n
 *   \li \p logTEST    (3) : software test activities.
 *   \li \p logDEBUG   (4) : debugging information.
 *   \li \p logEXTDBG  (5) : more detailed debugging information.
 *
 *   Convenient macros are provided in log.h for logging standard and
 *   verbose information .
 */

/* 
 * System Headers 
 */
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <syslog.h>
#include <time.h>
#include <sys/time.h>

/*
 * MCS Headers 
 */
#include "mcs.h"

/*
 * Local Headers 
 */
#include "log.h"
#include "logPrivate.h"

/*
 * Default initialization
 */
static logRULE logRule = {
    mcsTRUE,
    mcsTRUE,
    logWARNING,
    logWARNING,
    logWARNING,
    mcsTRUE,
    mcsTRUE
};
static logRULE *logRulePtr = &logRule;

/**
 * Log data to logsystem if level satisfies and optionally
 * to the stdout if verbose is specified.
 * 
 * \param modName name of the module relative.
 * \param level of message.
 * \param fileLine fileLine pointing source code of message.
 * \param logFormat format of given message.
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logPrint( const mcsMODULEID modName, logLEVEL level,
                        const char *fileLine,
                        const char *logFormat, ...)
{
    char buffer[4*logTEXT_LEN];
    va_list         argPtr;
    mcsCOMPL_STAT   stat = SUCCESS;
    mcsBYTES32      infoTime;

    /* Get UNIX-style time and display as number and string. */
    logGetTimeStamp(infoTime);

    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel))
    {
        /* Log information */
        va_start(argPtr,logFormat);
        vsprintf(buffer, logFormat, argPtr);
        stat = logData(modName, level, infoTime, fileLine, buffer);
        va_end(argPtr);
    }
	
    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel))
    {
        fprintf(stdout, "%s - %s - ", mcsGetProcName(), modName);
        if (logRulePtr->printDate == mcsTRUE)
        {
            fprintf(stdout, "%s - ", infoTime);
        }
        if ((fileLine != NULL ) && (logRulePtr->printFileLine == mcsTRUE)) 
        {
            fprintf(stdout, "%s - ", fileLine);
        }
        va_start(argPtr,logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout,"\n"); fflush(stdout);
        va_end(argPtr);
    }
    /* End if */

    return stat;

}

/**
 * Print action log.
 *
 * \param level  TBD
 * \param logFormat msg 
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logPrintAction(logLEVEL level, const char *logFormat, ...)
{ 
    va_list         argPtr;
    mcsCOMPL_STAT   stat = SUCCESS;

    /* If the specified level is less than or egal to the action level */
    if (level <= logRulePtr->actionLevel)
    {
        /* do a simple output which could be filtered and displayed in
         * a special position color ...
         * \todo implement real function
         */
        printf("ACTION: ");
        va_start(argPtr,logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout,"\n"); 
        fflush(stdout);
        va_end(argPtr);
    }/* End if */

    return stat;
}


/**
 * Set logging level.
 *  
 * \param level TBD
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetLogLevel (logLEVEL level)
{
    /* Set the logging level to the specified one */
    logRulePtr->logLevel = level;

    return SUCCESS; 
}

/**
 * Toggle the log.
 *
 * \param flag mcsTRUE/mcsFALSE
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetLog(mcsLOGICAL flag)
{
   /* Set the logging mode to the specified one */
   logRulePtr->log = flag;
   return SUCCESS;
}


/**
 * Get logging level.
 *
 * \return logLEVEL actual logging level 
 */
logLEVEL logGetLogLevel ()
{
    /* Returns the logging level  */
    return (logRulePtr->logLevel);
}



/**
 * Toggle the verbosity.
 *
 * \param flag mcsTRUE/mcsFALSE
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetVerbose(mcsLOGICAL flag)
{

    /* Set the verbose mode to the specified one */
    logRulePtr->verbose = flag;
    return SUCCESS;

}

/**
 * Set verbosity level.
 *
 * \param level  
 *
 * \return mcsCOMPL_STAT
 */
mcsCOMPL_STAT logSetVerboseLevel (logLEVEL level)
{
    
    /* Set the verbosity level to the specified one */
    logRulePtr->verboseLevel = level;

    return SUCCESS; 

}


/**
 * Get verbosity level.
 */
logLEVEL logGetVerboseLevel ()
{

    /* Returns the verbosity level  */
    return (logRulePtr->verboseLevel);

}


/**
 * Set action level.
 * 
 * \param level TBD
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetActionLevel (logLEVEL level)
{
    /* Set the verbosity level to the specified one */
    logRulePtr->actionLevel = level;

    return SUCCESS; 
}


/**
 * Get action level.
 */
logLEVEL logGetActionLevel ()
{
    /* Returns the action level  */
    return (logRulePtr->actionLevel);
}

/**
 * Toggle the date output. Useful in test mode.
 * 
 * \param flag mcsTRUE/mcsFALSE
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL flag)
{
    /* Set 'print date' flag  */
    logRulePtr->printDate = flag;
    return SUCCESS;
}

/**
 * Toggle the fileline output. Useful in test mode.
 * 
 * \param flag mcsTRUE/mcsFALSE
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL flag)
{
    /* Set 'print line/file' flag  */
    logRulePtr->printFileLine = flag;
    return SUCCESS;
}

/**
 * Place msg into the logging system.
 * \todo fill for the future 
 *  
 * \param msg message to store.
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logData(const mcsMODULEID modName, 
                      logLEVEL level,
                      const char *timeStamp,
                      const char *fileLine,
                      const char *buffer)
{
    int priority;       /* syslog priority */
    static mcsLOGICAL logIsOpen = mcsFALSE;

    /* open syslog 
     * using by convention LOCAL3 facility
     * \todo use an environment variable to adjust the default LOCAL3 facility
     */
    if (logIsOpen == mcsFALSE)
    {
        openlog(mcsGetProcName(), LOG_NDELAY, LOG_LOCAL3);
        logIsOpen = mcsTRUE;
    }

    /* initialize priority according given loglevel */
    switch (level){
        case logERROR :     priority = LOG_ERR ;     break;
        case logQUIET :     priority = LOG_NOTICE ;     break;
        case logWARNING :   priority = LOG_WARNING ;    break;
        case logINFO :      priority = LOG_INFO ;       break;
        case logTEST :      priority = LOG_NOTICE;      break;
        case logDEBUG :	    priority = LOG_DEBUG;       break;
        case logEXTDBG :    priority = LOG_DEBUG;       break;
        default:            priority = LOG_INFO ;       break;
    }
    
    /* log to syslog system */
    syslog( priority, "%s %s %s %s", modName, timeStamp, fileLine, buffer);
        
    return SUCCESS;
}

/**
 * Format the current date and time, to be used as time stamp.
 *
 * This function generates the string corresponding to the current date,
 * expressed in Coordinated Universal Time (UTC), using the following format
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 *    
 *     2004-06-16T16:16:48.02941
 * 
 * \param timeStamp character array where the resulting date is stored
 */
void logGetTimeStamp(mcsBYTES32 timeStamp)
{
    struct timeval time;
    struct tm      *timeNow;
    mcsSTRING32    tmpBuf;

    /* Get local time */
    gettimeofday(&time, NULL);
 
    /* Format the date */
    timeNow = gmtime(&time.tv_sec);
    strftime(timeStamp, sizeof(mcsBYTES32), "%Y-%m-%dT%H:%M:%S", timeNow);
 
    /* Add ms and us */
    sprintf(tmpBuf, "%.6f", time.tv_usec/1e6);
    strcpy(tmpBuf, (tmpBuf + 1));
    strcat(timeStamp, tmpBuf);
}
