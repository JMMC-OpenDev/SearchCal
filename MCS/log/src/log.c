/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: log.c,v 1.23 2004-12-03 17:08:40 lafrasse Exp $"
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
* gluck     30-Jun-2004  Changed some APIs :
*                        logSetFileLogVerbosity -> logSetFileLogLevel
*                        logGetFileLogVerbosity -> logGetFileLogLevel
*                        logSetStdoutLogVerbosity -> logSetStdoutLogLevel
*                        logGetStdoutLogVerbosity -> logGetStdoutLogLevel
*                        logSetActionLogVerbosity -> logSetActionLogLevel
*                        logGetActionLogVerbosity -> logGetActionLogLevel
*                        Replaced logSetFileLogState by logEnableFileLog and
*                        logDisableFileLog
*                        Replaced logSetStdoutLogState by logEnableStdoutLog and
*                        logDisableStdoutLog
* lafrasse  03-Aug-2004  Factorized log messages generation in LogPrint
*                        Changed logData to remotely log with logManager
*                        Added logSetLogManagerHostName and
*                        logSetLogManagerPortNumber functions
* lafrasse  10-Aug-2004  Moved logGetTimeStamp back in
*                        Changed back to logData original API
* gzins     10-Nov-2004  Replaced logDisplayError by logPrintErrMessage
*
*
*******************************************************************************/


/**
 * \file
 * The 'log' library provides functions that enable users to handle the three
 * types of logs for event logging.  The three kinds of logs contain the same
 * information, but are stored in different locations, as shown thereafter :
 *   \li File Logs are stored into standard MCS logManager files
 *   \li Stdout Logs are written to stdout
 *   \li Action Logs - TBD\n\n
 *
 * The levels of information are controlled
 * individually by :
 *   \li logSetFileLogLevel()
 *   \li logSetStdoutLogLevel()
 *   \li logSetActionLogLevel()\n\n
 *
 * Current log levels can be retrieved with :
 *   \li logGetFileLogLevel()
 *   \li logGetStdoutLogLevel()
 *   \li logGetActionLogLevel()\n\n
 *
 * Logging levels range from \p logQUIET to \p logEXTDBG , where the lowest
 * number means the lowest priority. By default, each log level is set to
 * \p logINFO. The following enums should be used to set the log levels :
 *   \li \p logQUIET   : no echo
 *   \li \p logWARNING : errors or abnormal events for application
 *   \li \p logINFO    : major events
 *   \li \p logTEST    : software test activities
 *   \li \p logDEBUG   : debugging information
 *   \li \p logEXTDBG  : more detailed debugging information\n\n
 *
 * The following convenient macros are provided in log.h for file and stdout
 * logging :\n
 *   \li logWarning()
 *   \li logInfo()
 *   \li logTest()
 *   \li logDebug()
 *   \li logExtDbg()\n\n
 *
 * \warning
 * The file used to store log messages is managed by the logManager process\n\n
 *
 * \sa logManager.c documentation\n\n
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
 *      logExtDbg("mymodPrint()");
 *
 *      if (param == NULL)
 *      {
 *          logWarning("Parameter is a null pointer. Do nothing!");
 *          return FAILURE;
 *      }
 *      printf("%s\n", param); 
 *
 *      return SUCCESS;
 *  }
 *  
 *  int main(int argc, char *argv[])
 *  {
 *      mcsInit(argv[0]);
 *
 *      logInfo("Main starting...");
 *
 *      mymodPrint("My message");
 *
 *      logInfo("Main exiting ..");
 *      exit(EXIT_SUCCESS);
 *  }
 * \endcode
 */

 
/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include <netinet/in.h>
#include <sys/utsname.h>
#include <netdb.h>

#include <errno.h>

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
 * Default initialization for logging levels, printtime, and so on.
 *
 * logRULE is defined in logPrivate.h
 */
static logRULE logRule =
{
    "\0",
    logMANAGER_DEFAULT_PORT_NUMBER,
    mcsTRUE,
    mcsTRUE,
    logINFO,
    logINFO,
    logINFO,
    mcsTRUE,
    mcsTRUE
};

/* Global pointing to the default log library configuration */
static logRULE *logRulePtr = &logRule;


/* Global holding the connection to logManager */
static mcsLOGICAL logSocketIsAlreadyOpen = mcsFALSE;


/*
 * Public Functions
 */

/**
 * Redefine the logManager host name (local host name by default) to be used for
 * the connection to the logManager daemon.
 *
 * If this function is not called, the local host name will be used by default.
 *
 * \param hostName a NULL-terminated string containing the desired host name
 *
 * \return FAILURE if the connection to logManger has already been opened (and
 * thus cannot be changed anymore) or if the given host name value seems bad,
 * SUCCESS otherwise
 */
mcsCOMPL_STAT logSetLogManagerHostName(mcsBYTES256 hostName)
{
    /* If the socket to the logManager has already been opened... */
    if (logSocketIsAlreadyOpen == mcsTRUE)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the connection to it is already opened");
        return FAILURE;
    }

    /* If the given host name seems bad... */
    if ((hostName == NULL) || (strlen(hostName) == 0))
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the received parameter seems bad");
        return FAILURE;
    }

    /* Store the desired logManager host name */
    strcpy(logRulePtr->logManagerHostName, hostName);

    return SUCCESS;
}


/**
 * Redefines the logManager port number (8791 by default) to be used for
 * the connection to the logManager daemon.
 *
 * If this function is not called, the logManager default port number (8791)
 * will be used.
 *
 * \param portNumber an integer containing the desired port number
 *
 * \return FAILURE if the connection to logManger has already been opened (and
 * thus cannot be changed anymore) or if the given port number value seems bad,
 * SUCCESS otherwise
 */
mcsCOMPL_STAT logSetLogManagerPortNumber(mcsUINT32 portNumber)
{
    /* If the socket to the logManager has already been opened... */
    if (logSocketIsAlreadyOpen == mcsTRUE)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the connection to it is already opened");
        return FAILURE;
    }

    /* If the given port number is out of range... */
    if (portNumber < 0 || portNumber > 65535)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the received parameter is out of range");
        return FAILURE;
    }

    /* Store the desired logManager host name */
    logRulePtr->logManagerPortNumber = portNumber;

    return SUCCESS;
}


/**
 * Switch file logging ON.
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT logEnableFileLog()
{
   /* Switch ON the file logging */
   logRulePtr->log = mcsTRUE;

   return SUCCESS;
}


/**
 * Switch file logging OFF.
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT logDisableFileLog()
{
   /* Switch OFF the file logging */
   logRulePtr->log = mcsFALSE;

   return SUCCESS;
}


/**
 * Set the file logging level.
 *
 * The level which is set is one of the enumeration type logLEVEL
 *
 * \param level Required log level (verbosity level)
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT logSetFileLogLevel (logLEVEL level)
{
    /* Set the file logging level to the specified one */
    logRulePtr->logLevel = level;

    return SUCCESS; 
}


/**
 * Get the file logging level.
 *
 * The level which is get is one of the enumeration type logLEVEL
 * 
 * \return logLEVEL actual file logging level (verbosity level)
 */
logLEVEL logGetFileLogLevel ()
{
    /* Returns the file logging level */
    return (logRulePtr->logLevel);
}


/**
 * Switch stdout logging ON.
 *
 * \return always SUCCESS 
 */
mcsCOMPL_STAT logEnableStdoutLog ()
{
   /* Switch ON the stdout logging */
    logRulePtr->verbose = mcsTRUE;

    return SUCCESS;
}


/**
 * Switch stdout logging OFF.
 *
 * \return always SUCCESS 
 */
mcsCOMPL_STAT logDisableStdoutLog ()
{
   /* Switch OFF the stdout logging */
    logRulePtr->verbose = mcsFALSE;

    return SUCCESS;
}


/**
 * Set the stdout logging level.
 *
 * The level which is set is one of the enumeration type logLEVEL
 *
 * \param level Required log level (verbosity level)
 *
 * \return always SUCCESS 
 */
mcsCOMPL_STAT logSetStdoutLogLevel (logLEVEL level)
{
    /* Set the stdout logging level to the specified one */
    logRulePtr->verboseLevel = level;

    return SUCCESS; 
}


/**
 * Get the stdout logging level.
 *
 * The level which is get is one of the enumeration type logLEVEL
 * 
 * \return logLEVEL actual stdout logging level (verbosity level)
 */
logLEVEL logGetStdoutLogLevel ()
{
    /* Returns the stdout logging level */
    return (logRulePtr->verboseLevel);
}


/**
 * Set the action logging level.
 * 
 * The level which is set is one of the enumeration type logLEVEL
 * 
 * \param level Required log level (verbosity level)
 *
 * \return always SUCCESS 
 */
mcsCOMPL_STAT logSetActionLogLevel (logLEVEL level)
{
    /* Set the action logging level to the specified one */
    logRulePtr->actionLevel = level;

    return SUCCESS; 
}


/**
 * Get the action logging level.
 *
 * The level which is get is one of the enumeration type logLEVEL
 * 
 * \return logLEVEL actual file logging level (verbosity level) 
 */
logLEVEL logGetActionLogLevel ()
{
    /* Returns the action logging level */
    return (logRulePtr->actionLevel);
}


/**
 * Switch ON/OFF the date output (useful in test mode).
 * 
 * \param flag mcsTRUE to turn date print ON, mcsFALSE otherwise
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
 * Switch ON/OFF the fileline output (seful in test mode).
 * 
 * \param flag mcsTRUE to turn fileline print ON, mcsFALSE otherwise
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
 * Log data to logsystem if level satisfies, and optionally to the stdout if
 * verbose is specified.
 * 
 * \param modName name of the module relative.
 * \param level of message.
 * \param fileLine fileLine pointing source code of message.
 * \param logFormat format of given message.
 * 
 * \return mcsCOMPL_STAT 
 */
mcsCOMPL_STAT logPrint(const mcsMODULEID modName, logLEVEL level,
                       const char *fileLine, const char *logFormat, ...)
{
    va_list         argPtr;

    mcsBYTES32      infoTime;
    char buffer[4*logTEXT_LEN];
    buffer[0] = '\0';

    mcsCOMPL_STAT   stat = SUCCESS;

    /* Get UNIX-style time and display as number and string. */
    logGetTimeStamp(infoTime);

    /* If the log message should be file-logged, and that its log level is less
     * than or egal to the desired file-logging level
     */
    if ((logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel))
    {
        /* Log information to file */
        va_start(argPtr,logFormat);
        vsprintf(buffer, logFormat, argPtr);
        stat = logData(modName, level, infoTime, fileLine, buffer);
        va_end(argPtr);
    }
	
    /* If the log message should be stdout logged, and that its log level is
     * less than or egal to the desired stdout logging level
     */
    if ((logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel))
    {
        /* Print the log message header */
        fprintf(stdout, "%s - %s - %s - ", mcsGetEnvName(), mcsGetProcName(),
                modName);
    
        /* If the log message should contain the date */ 
        if (logRulePtr->printDate == mcsTRUE)
        {
            /* Print it */
            fprintf(stdout, "%s - ", infoTime);
        }
    
        /* If the fileline exists and should be contained in the log message */
        if ((fileLine != NULL ) && (logRulePtr->printFileLine == mcsTRUE)) 
        {
            /* Print it */
            fprintf(stdout, "%s - ", fileLine);
        }
    
        /* Compute the variable parameters and print them */
        va_start(argPtr, logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout, "\n");
        fflush(stdout);
        va_end(argPtr);
    }

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

        va_start(argPtr, logFormat);
        vfprintf(stdout, logFormat, argPtr);
        fprintf(stdout, "\n"); 
        fflush(stdout);
        va_end(argPtr);
    }

    return stat;
}


/**
 * Place a message into the file logging system.
 *  
 * \param modName name of the module relative.
 * \param level level of message.
 * \param timeStamp time stamp of the message.
 * \param fileLine file name and line number from where the message is issued.
 * \param logText message to be logged.
 * 
 * \return SUCCESS.
 */
mcsCOMPL_STAT logData(const mcsMODULEID modName, logLEVEL level,
                      const char *timeStamp, const char *fileLine,
                      const char *logText)
{
    /* Socket stuff */
    static int sock = 0;
    static struct sockaddr_in server;
    struct hostent *hp = NULL;

    /* Message formating gstuff */
    mcsBYTES32      infoTime;
    char            *priorityMsg = NULL;
    char            logMsg[4*logTEXT_LEN];
    logMsg[0]       = '\0';

    /* Get UNIX-style time and display as number and string. */
    logGetTimeStamp(infoTime);

    /* initialize priority according given loglevel */
    switch (level){
        case logERROR:      priorityMsg = "Error";      break;
        case logQUIET:      priorityMsg = "Quiet";      break;
        case logWARNING:    priorityMsg = "Warning";    break;
        case logINFO:       priorityMsg = "Info" ;      break;
        case logTEST:       priorityMsg = "Test";       break;
        case logDEBUG:	    priorityMsg = "Debug";      break;
        case logEXTDBG:     priorityMsg = "Ext Dbg";    break;
        default:            priorityMsg = "Info";       break;
    }
    
    /* Compute the log message */
    sprintf(logMsg, "%s - %s - %s - %s - %s - %s - %s", mcsGetEnvName(),
            mcsGetProcName(), modName, priorityMsg, infoTime, fileLine,
            logText);

    /* If the connection to the logManager is NOT already opened, open it */
    if (logSocketIsAlreadyOpen == mcsFALSE)
    {
        /* If no specific logManager host name has not been redefined by the log
         * library user...
         */
        if (strlen(logRulePtr->logManagerHostName) == 0)
        {
            /* Try to get the local host name */
            if (logGetHostName(logRulePtr->logManagerHostName,
                               sizeof(logRulePtr->logManagerHostName))
                == FAILURE)
            {
                logPrintErrMessage("- LOG LIBRARY ERROR - logGetHostName() failed - %s", strerror(errno));
                return FAILURE;
            }
            
            /* If the local host name seems empty... */
            if ((logRulePtr->logManagerHostName == NULL)
                || (strlen(logRulePtr->logManagerHostName) == 0))
            {
                logPrintErrMessage("- LOG LIBRARY ERROR - got an empty hostname");
                return FAILURE;
            }
        }

        /* Try to create ths socket */
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1) 
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - socket() failed - %s",
                    strerror(errno));
            return FAILURE;
        }

        hp = gethostbyname(logRulePtr->logManagerHostName);
        if (hp == NULL )
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - gethostbyname(%s) failed", logRulePtr->logManagerHostName);
            return FAILURE;
        }

        /* Copy the resolved information into the sockaddr_in structure */
        memset(&server, '\0', sizeof(server));
        memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
        server.sin_family = hp->h_addrtype;
        server.sin_port   = htons(logRulePtr->logManagerPortNumber);

        logSocketIsAlreadyOpen = mcsTRUE;
    }

    /* Send message to the logManager process */
    if (sendto(sock, (void *)logMsg, strlen(logMsg), 0,
               (const struct sockaddr *)&server, sizeof(server)) == -1)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - sendto() failed - %s",
                strerror(errno));
        return FAILURE;
    }

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


/*___oOo___*/
