/*******************************************************************************
*  JMMC Project
*  
*  "@(#) $Id: log.c,v 1.7 2004-05-27 13:18:20 mella Exp $"
*
* who       when      what
* --------  --------  ----------------------------------------------
* mella   07/05/04  Preliminary version based on log from VLT/ESO
=* 
 */

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

Log is a C module for logging to syslog or stdout. It is a module of the MCS software.
Log is available from the CVS repository of JMMC or is included in the MCS distribution. 

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

This library generates log messages using syslog(), which will  be  distributed  by  syslogd.

The following softwares are needed to use the log library:

\li syslogd

\section installation Installation
 
\subsection installation_cvs Building from cvs's sources

The log module uses the MCS makefile system. The following commands should build log on the supported platforms:

\code

$ cvs co log
$ cd log/src
$ make

\endcode

The following command should install the log library into the INTROOT directory defined by the environment variable $INTROOT:

\code

$ make install

\endcode

\section config Configuration

\subsection conf_files Configuration files

\subsection conf_syntax Configuration syntax

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
 *   The logging levels range from \p logQUIET to \p logEXTDBG , where the lowest 
 *   number means the  lowest priority. By default, the level of all the 
 *   logs is set to \p logWARNING.
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

static mcsPROCNAME logProcName = "procUNKNOWN";

/**
 * Get names to identify the process. The module name must be given for each
 * logging call. If process is not identified, then default name is procUNKNOWN
 *
 * \param processName name of the process.
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logIdentify(const mcsPROCNAME processName){
    /* store values into global variables */
    strncpy(logProcName, processName, mcsPROCNAME_LEN);

    /* open syslog 
     * using by convention LOCAL3 facility
     * \todo use an environment variable to adjust the default LOCAL3 facility
     */
    openlog(processName, LOG_NDELAY, LOG_LOCAL3);
        
    return SUCCESS;
}

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

    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel))
    {
        /* Log information */
        va_start(argPtr,logFormat);
        vsprintf(buffer, logFormat, argPtr);
        stat = logData(modName, level, fileLine, buffer);
        va_end(argPtr);
    }
	
    /* If the specified level is less than or egal to the logging level */
    if ((logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel))
    {
        fprintf(stdout, "%s - %s - ", logProcName, modName);
        if ( (fileLine != NULL ) && (logRulePtr->printFileLine == mcsTRUE)) 
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
static mcsCOMPL_STAT logData(const mcsMODULEID modName, logLEVEL level,
                        const char *fileLine,
                        const char *buffer)
{
    int priority;       /* syslog priority */

    /* initialize priority according given loglevel */
    switch (level){
        case logQUIET :     priority = LOG_NOTICE ;     break;
        case logWARNING :   priority = LOG_WARNING ;    break;
        case logINFO :      priority = LOG_INFO ;       break;
        case logTEST :      priority = LOG_NOTICE;      break;
        case logDEBUG :	    priority = LOG_DEBUG;       break;
        case logEXTDBG :    priority = LOG_DEBUG;       break;
        default:            priority = LOG_INFO ;       break;
    }
    
    /* log to syslog system */
    syslog( priority, "%s %s %s", modName, fileLine, buffer);
        
    return SUCCESS;
}



