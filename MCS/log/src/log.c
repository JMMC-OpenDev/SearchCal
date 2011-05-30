/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Main code file, providing all the functions that enable your code to handle
 * file and stdout logging, with different levels of verbosity for each output.
 *
 * The 2 kinds of logs output store the informations in different locations, as
 * shown thereafter :
 *   \li \em File logs are stored into standard MCS log files (see below);
 *   \li \em Stdout logs are written on the standard output (e.g console).
 *
 * Each log output can have its own log verbosity level, ranging from
 * \p logQUIET (no output at all) to \p logTRACE (the most detailed log level,
 * containing all the available informations).\n
 * By default, each output log level is set to \p logINFO. The following values
 * are also available to specify any desired log level :
 *   \li \p logQUIET   : nothing is logged;
 *   \li \p logWARNING : only errors and abnormal events are logged;
 *   \li \p logINFO    : same as above, plus major events logging;
 *   \li \p logTEST    : same as above, plus software test activities logging;
 *   \li \p logDEBUG   : same as above, plus debugging information logging;
 *   \li \p logTRACE   : same as above, plus function/method trace logging. 
 *
 * For each output, the desired level of information is set with :
 *   \li logSetFileLogLevel();
 *   \li logSetStdoutLogLevel().
 *
 * For each output, the current log level can be retrieved with :
 *   \li logGetFileLogLevel();
 *   \li logGetStdoutLogLevel().
 *
 * The following convenient macros are provided in log.h for logging
 * informations (see log.h documentation for more details) :
 *   \li logWarning();
 *   \li logInfo();
 *   \li logTest();
 *   \li logDebug();
 *   \li logTrace().
 * \n\n
 *
 * \b Files:
 *   \li \e \<$MCSDATA/log/logfile\> : contains the logged informations
 * \n\n
 *
 * \sa log.h documentation
 * \sa logManager.c documentation\n\n
 * \n
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
 *      /# Logged only on outputs that use the logTRACE log level #/
 *      logTrace("mymodPrint()");
 *
 *      if (param == NULL)
 *      {
 *          /# Logged only on outputs that use the logWARNING or greater #/
 *          logWarning("Parameter is a null pointer. Do nothing!");
 *          return mcsFAILURE;
 *      }
 *      printf("%s\n", param); 
 *
 *      return mcsSUCCESS;
 *  }
 *  
 *  int main(int argc, char *argv[])
 *  {
 *      mcsInit(argv[0]);
 *
 *      /# Set the file output log level to logTRACE (most detailed level) #/
 *      logSetFileLogLevel(logTRACE);
 *
 *      /# Logged only on outputs that use the logINFO or greater log level #/
 *      logInfo("Main starting...");
 *
 *      mymodPrint("My message");
 *
 *      /# Logged only on outputs that use the logINFO or greater log level #/
 *      logInfo("Main exiting ..");
 *
 *      exit(EXIT_SUCCESS);
 *  }
 * \endcode
 *
 * The result of the execution of this program (logExample) should look like
 * this on stdout :
 * \code
 * default - logExample - mymod - 2005-01-26T16:03:24.831050 - logExample.c:68 - Main starting...
 * My message
 * default - logExample - mymod - 2005-01-26T16:03:24.832196 - logExample.c:73 - Main exiting ..
 * \endcode
 *
 * The result of the execution of this program (logExample) should look like
 * this in file :
 * \code
 * default - logExample - mymod - Info - 2005-01-26T16:08:37.585898 - logExample.c:68 - Main starting...
 * default - logExample - mymod - Trace - 2005-01-26T16:08:37.586787 - logExample.c:40 - mymodPrint()
 * default - logExample - mymod - Info - 2005-01-26T16:08:37.586820 - logExample.c:73 - Main exiting ..
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

/* Number of modules in the allowed modules list */ 
static mcsINT32 logNbAllowedMod = 0;

/* List of allowed modules */
#define logNB_MAX_ALLOWED_MOD 20
static mcsMODULEID logAllowedModList[logNB_MAX_ALLOWED_MOD];

/* Global pointing to the default log library configuration */
static logRULE *logRulePtr = &logRule;

/* Global holding the connection to logManager */
static mcsLOGICAL logSocketIsAlreadyOpen = mcsFALSE;


/*
 * Public Functions
 */

/**
 * Redefine the logManager host name (\e localhost by default) to be used for
 * the connection to the logManager daemon.
 *
 * If this function is not called, \e localhost will be used by default.
 *
 * \param hostName a NULL-terminated string containing the desired host name
 *
 * \return mcsFAILURE if the connection to logManger has already been opened
 * (and thus cannot be changed anymore), or if the given host name parameter
 * seems wrong, mcsSUCCESS otherwise
 */
mcsCOMPL_STAT logSetLogManagerHostName(mcsBYTES256 hostName)
{
    /* If the socket to the logManager has already been opened... */
    if (logSocketIsAlreadyOpen == mcsTRUE)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the connection to it is already opened");
        return mcsFAILURE;
    }

    /* If the given host name seems wrong... */
    if ((hostName == NULL) || (strlen(hostName) == 0))
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the received parameter seems bad");
        return mcsFAILURE;
    }

    /* Store the desired logManager host name */
    strcpy(logRulePtr->logManagerHostName, hostName);

    return mcsSUCCESS;
}


/**
 * Redefines the logManager port number (\e 8791 by default) to be used for
 * the connection to the logManager daemon.
 *
 * If this function is not called, the logManager default port number (\e 8791)
 * will be used.
 *
 * \param portNumber an integer containing the desired IPv4 port number
 *
 * \return mcsFAILURE if the connection to logManger has already been opened
 * (and thus cannot be changed anymore), or if the given port number parameter
 * seems wrong, mcsSUCCESS otherwise
 */
mcsCOMPL_STAT logSetLogManagerPortNumber(mcsUINT32 portNumber)
{
    /* If the socket to the logManager has already been opened... */
    if (logSocketIsAlreadyOpen == mcsTRUE)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the connection to it is already opened");
        return mcsFAILURE;
    }

    /* If the given port number is out of range... */
    if (portNumber < 0 || portNumber > 65535)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the received parameter is out of range");
        return mcsFAILURE;
    }

    /* Store the desired logManager host name */
    logRulePtr->logManagerPortNumber = portNumber;

    return mcsSUCCESS;
}


/**
 * Switch file logging ON.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logEnableFileLog()
{
   /* Switch ON the file logging */
   logRulePtr->log = mcsTRUE;

   return mcsSUCCESS;
}


/**
 * Switch file logging OFF.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logDisableFileLog()
{
   /* Switch OFF the file logging */
   logRulePtr->log = mcsFALSE;

   return mcsSUCCESS;
}


/**
 * Set the file logging level as defined in the logLEVEL enumeration (logINFO by 
 * default).
 *
 * \param level Desired log level (verbosity), as defined in the logLEVEL
 * enumeration
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logSetFileLogLevel (logLEVEL level)
{
    /* Set the file logging level to the specified one */
    logRulePtr->logLevel = level;

    return mcsSUCCESS; 
}


/**
 * Get the file logging level, as defined in the logLEVEL enumeration.
 * 
 * \return current file logging level (verbosity), as defined in the logLEVEL
 * enumeration
 */
logLEVEL logGetFileLogLevel ()
{
    /* Returns the file logging level */
    return (logRulePtr->logLevel);
}


/**
 * Switch stdout logging ON.
 *
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logEnableStdoutLog ()
{
   /* Switch ON the stdout logging */
    logRulePtr->verbose = mcsTRUE;

    return mcsSUCCESS;
}


/**
 * Switch stdout logging OFF.
 *
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logDisableStdoutLog ()
{
   /* Switch OFF the stdout logging */
    logRulePtr->verbose = mcsFALSE;

    return mcsSUCCESS;
}


/**
 * Set the stdout logging level as defined in the logLEVEL enumeration (logINFO
 * by default).
 *
 * \param level Desired log level (verbosity), as defined in the logLEVEL 
 * enumeration
 *
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logSetStdoutLogLevel (logLEVEL level)
{
    /* Set the stdout logging level to the specified one */
    logRulePtr->verboseLevel = level;

    return mcsSUCCESS; 
}

/**
 * Get the stdout logging level, as defined in the logLEVEL enumeration.
 *
 * \return current stdout logging level (verbosity),  as defined in the logLEVEL
 * enumeration
 */
logLEVEL logGetStdoutLogLevel ()
{
    /* Returns the stdout logging level */
    return (logRulePtr->verboseLevel);
}

/**
 * Clear the list of 'allowed-to-log' modules.
 *
 * Clear the list of modules which are allowed to log informations on stdout. 
 * After the list has been cleared, the filtering is off; i.e all logs are 
 * printed (according to the current log level) on stdout. 
 * 
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logClearStdoutLogAllowedModList(void)
{
    /* Reset the number of allowed modules */
    logNbAllowedMod = 0;

    return mcsSUCCESS; 
}

/**
 * Add a module to the list of allowed modules.
 *
 * Add a module to the list of modules which are allowed to print out on
 * stdout logging, thus activating module filtering.
 * 
 * \param mod Name of the module to be added to the list of allowed modules
 *
 * \return mcsSUCCESS or mcsFAILURE if the list is full. 
 */
mcsCOMPL_STAT logAddToStdoutLogAllowedModList(char *mod)
{
    /* Check if table is full */
    if (logNbAllowedMod == logNB_MAX_ALLOWED_MOD)
    {
        return mcsFAILURE;
    }

    /* Add module to the list */
    strncpy(logAllowedModList[logNbAllowedMod], mod, mcsMODULEID_LEN);
    logNbAllowedMod++;

    return mcsSUCCESS; 
}

/**
 * Switch ON/OFF the date output (useful in test mode).
 * 
 * \param flag mcsTRUE to turn date printing ON, mcsFALSE otherwise
 *
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL flag)
{
    /* Set 'print date' flag  */
    logRulePtr->printDate = flag;

    return mcsSUCCESS;
}

/**
 * Return whether date output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if date printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintDate()
{
    return logRulePtr->printDate;
}


/**
 * Switch ON/OFF the fileline output (useful in test mode).
 * 
 * \param flag mcsTRUE to turn fileline printing ON, mcsFALSE otherwise
 *
 * \return always mcsSUCCESS 
 */
mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL flag)
{
    /* Set 'print line/file' flag  */
    logRulePtr->printFileLine = flag;

    return mcsSUCCESS;
}

/**
 * Return whether fileline output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if fileline printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintFileLine()
{
    return logRulePtr->printFileLine;
}


/**
 * Log informations into file and stdout, according to the specified log level.
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
    mcsCOMPL_STAT status = mcsSUCCESS;

    /* fast return if this log message is discarded */
    if ( !( (logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel) )
            && 
         !( (logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel) ) )
    {
        return status;
    }

    va_list argPtr;
    mcsBYTES32 infoTime;

    /* Get UNIX-style time and display as number and string. */
    logGetTimeStamp(infoTime);

    /* If the log message should be file-logged, and that its log level is less
     * than or egal to the desired file-logging level
     */
    if ((logRulePtr->log == mcsTRUE) && (level <= logRulePtr->logLevel))
    {
        /* Log information to file */
        va_start(argPtr,logFormat);
        
        char buffer[8192];
        buffer[0] = '\0';
        
        vsprintf(buffer, logFormat, argPtr);
        status = logData(modName, level, infoTime, fileLine, buffer);
        va_end(argPtr);
    }
	
    /* If the log message should be stdout logged, and that its log level is
     * less than or egal to the desired stdout logging level
     */
    if ((logRulePtr->verbose == mcsTRUE) && (level <= logRulePtr->verboseLevel))
    {
        /* Check if module belongs to the list of allowed modules */
        mcsLOGICAL allowed;
        if (logNbAllowedMod != 0)
        {
            int i;
            allowed = mcsFALSE;
            for (i=0; (i<logNbAllowedMod) && (allowed == mcsFALSE); i++)
            {
                if (strcmp(logAllowedModList[i], modName) == 0)
                {
                    allowed = mcsTRUE;
                }
            }
        }
        else
        {
            allowed = mcsTRUE;
        }

        /* If message can be printed out */
        if (allowed == mcsTRUE)
        {
            /* Print the log message header */
            fprintf(stdout, "%s - %s - ",
                    mcsGetProcName(), modName);
            fflush(stdout);

            /* If the log message should contain the date */ 
            if (logRulePtr->printDate == mcsTRUE)
            {
                /* Print it */
                fprintf(stdout, "%s - ", infoTime);
                fflush(stdout);
            }

            /* If the fileline exists and should be contained in the log
             * message */
            if ((fileLine != NULL ) && (logRulePtr->printFileLine == mcsTRUE)) 
            {
                /* Print it */
                fprintf(stdout, "%s - ", fileLine);
                fflush(stdout);
            }

            /* Compute the variable parameters and print them */
            va_start(argPtr, logFormat);
            vfprintf(stdout, logFormat, argPtr);
            fprintf(stdout, "\n");
            fflush(stdout);
            va_end(argPtr);
        }
        /* End if */
    }

    return status;
}


/**
 * Log informations into file only, according to the specified log level.
 *  
 * \param modName name of the module relative.
 * \param level level of message.
 * \param timeStamp time stamp of the message.
 * \param fileLine file name and line number from where the message is issued.
 * \param logText message to be logged.
 * 
 * \return mcsSUCCESS.
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
    char            logMsg[8192];
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
        case logTRACE:      priorityMsg = "Trace";    break;
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
                == mcsFAILURE)
            {
                logPrintErrMessage("- LOG LIBRARY ERROR - logGetHostName() failed - %s", strerror(errno));
                return mcsFAILURE;
            }
            
            /* If the local host name seems empty... */
            if ((logRulePtr->logManagerHostName == NULL)
                || (strlen(logRulePtr->logManagerHostName) == 0))
            {
                logPrintErrMessage("- LOG LIBRARY ERROR - got an empty hostname");
                return mcsFAILURE;
            }
        }

        /* Try to create ths socket */
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1) 
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - socket() failed - %s",
                    strerror(errno));
            return mcsFAILURE;
        }

        hp = gethostbyname(logRulePtr->logManagerHostName);
        if (hp == NULL )
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - gethostbyname(%s) failed", logRulePtr->logManagerHostName);
            return mcsFAILURE;
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
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Return the current date and time, to be used as time stamp in logs.
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
 
    /* Add milli-second and micro-second */
    sprintf(tmpBuf, "%.6f", time.tv_usec/1e6);
    strcat(timeStamp, &tmpBuf[1]);
}


/*___oOo___*/
