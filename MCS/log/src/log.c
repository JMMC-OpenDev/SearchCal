/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: log.c,v 1.17 2004-07-30 14:34:58 lafrasse Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* mella     07-May-2004  Created preliminary version based on log from VLT/ESO
* gzins     16-Jun-2004  Removed logIdentify function; replaced by mcsInit and
*                        mcsGetProcName functions
* lafrasse  30-Jun-2004  Changed some APIs :
*                        logSetLog -> logSetFileLogState
*                        logSetLogLevel -> logSetFileLogVerbosity
*                        logGetLogLevel -> logGetFileLogVerbosity
*                        logSetVerbose -> logSetStdoutLogState
*                        logSetVerboseLevel -> logSetStdoutLogVerbosity
*                        logGetVerboseLevel -> logGetStdoutLogVerbosity
*                        logSetActionLevel -> logSetActionLogVerbosity
*                        logGetActionLevel -> logGetActionLogVerbosity
*
*
*******************************************************************************/
/**
 * \mainpage Log : MCS C Logging module
 * \section summary Summary
 * This module provides functions that enable users to handle the three types
 * of logs for the Event Logging. The types of logs are:
 *   \li File Logs   : These logs are stored into standard MCS logManager files.
 *   \li Stdout Logs : These logs are written to stdout.
 *   \li Action Logs : TBD.
 *
 * The three kinds of logs contain the same information, but are stored
 * in different locations and the levels of information are controlled
 * individually by logSetFileLogVerbosity(), logSetStdoutLogVerbosity() and
 * logSetActionLogVerbosity() methods. The current log levels can be retrieved
 * by logGetFileLogVerbosity(), logGetStdoutLogVerbosity() and
 * logGetActionLogVerbosity() methods.
 *
 * The logging levels range from \p logQUIET to \p logEXTDBG , where the
 * lowest number means the lowest priority. By default, the level of all the
 * logs is set to \p logINFO.
 * The log and verbose flags indicate if the log information are presented
 * respectively on the standard \p MCS logger or on the stdout device
 * (default is mcsTRUE).
 * The following enums should be used to set the log levels : \n
 *   \li \p logQUIET   : no echo \n
 *   \li \p logWARNING : errors or abnormal events for application.\n
 *   \li \p logINFO    : major events.\n
 *   \li \p logTEST    : software test activities.
 *   \li \p logDEBUG   : debugging information.
 *   \li \p logEXTDBG  : more detailed debugging information.
 *
 * The following convenient macros are provided in log.h for logging
 * standard and verbose information : logWarning(), logInfo(), logTest(),
 * logDebug() and logExtDbg().
 *
 * \section config Configuration
 * This modules makes call to the syslogd daemon. syslogd must be configured to
 * route the messages to the correct destination.
 * 
 * \subsection conf_files Configuration files
 * In general, \p /etc/syslog.conf configures the sylogd. This file is taken
 * into account at startup or after a SIGHUP signal sent. \p /etc/init.d/syslogd
 * \p restart also makes \p syslogd reread configuration
 * file.  
 * 
 * \subsection conf_syntax Configuration syntax
 * Simplest configuration lines is:
 * \code
 * 
 * local3.*                            /home/MCS/mcs.log
 * 
 * \endcode
 *
 * See syslog.conf manual page for further information.
 *
 * \section Files
 * \li \c $MCS_LOG_FILES/logFile output file for normal logs
 *
 * \section Environment
 * \li \c VLT_LOG_FILES directory containing logging-files
 * 
 * \b Code \b Example:
* \code
 *  #include <stdio.h>
 *  #include <math.h>
 *
 *  #define MODULE_ID "mymod"
 *  #include "mcs.h"
 *  
 *  mcsCOMPL_STAT mymodPrint(char *param)
 *  {
 *      logExtDbg("mymodSqrt()");
 *
 *      if (param == NULL)
 *      {
 *          logWarning("Parameter is a null pointer. Do nothing!");
 *          return(SUCCESS);
 *      }
 *      printf("%s\n", param); 
 *
 *      return (SUCCESS);
 *  }
 *  
 *  int main(int argc, char *argv[])
 *  {
 *      mcsInit(argv[0]);
 *
 *      logInfo("Server starting ..");
 *
 *      mymodPrint("My message");
 *
 *      logInfo("Server exiting ..");
 *      exit (EXIT_SUCCESS);
 *  }
 * \endcode
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
 * Default initialization for logging levels printtime and so on..
 * logRULE is defined in logPrivate.h
 */
static logRULE logRule = {
    mcsTRUE,
    mcsTRUE,
    logINFO,
    logINFO,
    logINFO,
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
 * Switch file logging ON or OFF.
 *
 * \param flag mcsTRUE/mcsFALSE
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetFileLogState(mcsLOGICAL flag)
{
   /* Set the file logging state to the specified one */
   logRulePtr->log = flag;

   return SUCCESS;
}

/**
 * Set the file logging verbosity level.
 *  
 * \param level TBD
 *
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetFileLogVerbosity (logLEVEL level)
{
    /* Set the file logging verbosity level to the specified one */
    logRulePtr->logLevel = level;

    return SUCCESS; 
}

/**
 * Get the file logging verbosity level.
 *
 * \return logLEVEL actual file logging verbosity level 
 */
logLEVEL logGetFileLogVerbosity ()
{
    /* Returns the file logging verbosity level */
    return (logRulePtr->logLevel);
}



/**
 * Switch stdout logging ON or OFF.
 *
 * \param flag mcsTRUE/mcsFALSE
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetStdoutLogState(mcsLOGICAL flag)
{
   /* Set the stdout logging state to the specified one */
    logRulePtr->verbose = flag;

    return SUCCESS;
}

/**
 * Set the stdout logging verbosity level.
 *
 * \param level  
 *
 * \return mcsCOMPL_STAT
 */
mcsCOMPL_STAT logSetStdoutLogVerbosity (logLEVEL level)
{
    
    /* Set the stdout logging verbosity level to the specified one */
    logRulePtr->verboseLevel = level;

    return SUCCESS; 

}

/**
 * Get the stdout logging verbosity level.
 *
 * \return logLEVEL actual stdout logging verbosity level 
 */
logLEVEL logGetStdoutLogVerbosity ()
{
    /* Returns the stdout logging verbosity level */
    return (logRulePtr->verboseLevel);
}


/**
 * Set the action logging verbosity level.
 * 
 * \param level TBD
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logSetActionLogVerbosity (logLEVEL level)
{
    /* Set the action logging verbosity level to the specified one */
    logRulePtr->actionLevel = level;

    return SUCCESS; 
}

/**
 * Get the file logging verbosity level.
 *
 * \return logLEVEL actual file logging verbosity level 
 */
logLEVEL logGetActionLogVerbosity ()
{
    /* Returns the action logging verbosity level */
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
 *  
 * \param modName name of the module relative.
 * \param level level of message.
 * \param timeStamp time stamp of the message.
 * \param fileLine file name and line number from where the message is issued.
 * \param buffer message to be logged.
 * 
 * \return SUCCESS.
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
