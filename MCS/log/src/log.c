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
#include "pthread.h"


/*
 * MCS Headers
 */
#include "mcs.h"


/*
 * Local Headers
 */
#include "log.h"
#include "logPrivate.h"

/** flag to dump log thread context before free */
#define logTHREAD_CONTEXT_DUMP mcsFALSE

/** flag to log dynBuf size / realloc */
#define logTHREAD_LOG_BUFFER_SIZE mcsFALSE

/** thread local storage key for log thread context */
static pthread_key_t tlsKey_logContext;
/** flag to indicate that the thread local storage is initialized */
static mcsLOGICAL logInitialized = mcsFALSE;

/**
 * Log Thread context is a (simple) dynamic buffer per thread
 * (derived and simplified from miscDynBuf)
 */
typedef struct
{
    /* True if the log context is enabled */
    mcsLOGICAL  enabled;

    char       *dynBuf;           /**< A pointer to the Dynamic Buffer internal
                                     bytes buffer. */

    mcsUINT32   storedBytes;       /**< An unsigned integer counting the number
                                     of bytes effectively holden by Dynamic
                                     Buffer.
                                     */

    mcsUINT32   allocatedBytes;    /**< An unsigned integer counting the number
                                     of bytes already allocated in Dynamic
                                     Buffer. */
} logTHREAD_CONTEXT;

/*
 * To prevent concurrent access to shared ressources in multi-threaded context (socket initialization and setters).
 * Reads are not protected (dirty reads considered safe enough)
 */
static mcsMUTEX logMutex = MCS_MUTEX_STATIC_INITIALIZER;

/*
 * Local Macros
 */
/* log buffer capacity = 128K (stack) */
#define BUFFER_MAX_LEN (128 * 1024)

/* unlock log mutex and return given status */
#define UNLOCK_MUTEX_AND_RETURN_FAILURE() { \
    mcsMutexUnlock(&logMutex);              \
    return mcsFAILURE;                      \
}

/*
 * Default initialization for logging levels, printtime, and so on.
 *
 * logRULE is defined in logPrivate.h
 */
static logRULE logRule = {
                          "\0",
                          logMANAGER_DEFAULT_PORT_NUMBER,
                          mcsFALSE, /* disable socket logs */
                          mcsTRUE,  /*  enable stdout logs */
                          logINFO,
                          logINFO,
                          logINFO,
                          mcsTRUE,  /* print date */
                          mcsTRUE,  /* print file and line */
                          mcsFALSE, /* don't print process */
                          mcsFALSE, /* don't print module */
                          mcsFALSE  /* don't print thread */
};

/* Number of modules in the allowed modules list */
static mcsINT32 logNbAllowedMod = 0;

/* List of allowed modules */
#define logNB_MAX_ALLOWED_MOD 20
static mcsMODULEID logAllowedModList[logNB_MAX_ALLOWED_MOD];

/* Global pointing to the default log library configuration */
logRULE* logRulePtr = &logRule;

/* Global holding the connection to logManager */
static mcsLOGICAL logSocketIsAlreadyOpen = mcsFALSE;

/* Global Socket file descriptor to communicate with logManager */
static int sock = 0;
/* Global Socket to communicate with logManager */
static struct sockaddr_in server;

/*
 * Local Functions
 */
mcsCOMPL_STAT _logData(const mcsMODULEID, logLEVEL, const char *, const char *,
                       const char *logText);

logTHREAD_CONTEXT* logGetThreadContext();

mcsCOMPL_STAT logDynBufDestroy     (logTHREAD_CONTEXT *dynBuf);
mcsCOMPL_STAT logDynBufAppendString(logTHREAD_CONTEXT *dynBuf,
                                    const char        *str);
mcsCOMPL_STAT logDynBufAppendLine  (logTHREAD_CONTEXT *dynBuf,
                                    const char        *line);

/**
 * Fast strcat alternative (destination and source MUST not overlap)
 * No buffer overflow checks
 * @param dest destination pointer (updated when this function returns to indicate the position of the last character)
 * @param src source buffer
 */
char* strcatFast(char* dest, const char* src)
{
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return --dest;
}

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
mcsCOMPL_STAT logSetLogManagerHostName(mcsSTRING256 hostName)
{
    mcsMutexLock(&logMutex);

    /* If the socket to the logManager has already been opened... */
    if (IS_TRUE(logSocketIsAlreadyOpen))
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the connection to it is already opened");
        UNLOCK_MUTEX_AND_RETURN_FAILURE();
    }

    /* If the given host name seems wrong... */
    if (IS_NULL(hostName) || (strlen(hostName) == 0))
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager host name, as the received parameter seems bad");
        UNLOCK_MUTEX_AND_RETURN_FAILURE();
    }

    /* Store the desired logManager host name */
    strcpy(logRulePtr->logManagerHostName, hostName);

    mcsMutexUnlock(&logMutex);

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
    mcsMutexLock(&logMutex);

    /* If the socket to the logManager has already been opened... */
    if (IS_TRUE(logSocketIsAlreadyOpen))
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the connection to it is already opened");
        UNLOCK_MUTEX_AND_RETURN_FAILURE();
    }

    /* If the given port number is out of range... */
    if (portNumber > 65535)
    {
        logPrintErrMessage("- LOG LIBRARY ERROR - could not change logManager port number, as the received parameter is out of range");
        UNLOCK_MUTEX_AND_RETURN_FAILURE();
    }

    /* Store the desired logManager host name */
    logRulePtr->logManagerPortNumber = portNumber;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Switch file logging ON.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logEnableFileLog()
{
    mcsMutexLock(&logMutex);

    /* Switch ON the file logging */
    logRulePtr->log = mcsTRUE;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Switch file logging OFF.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logDisableFileLog()
{
    mcsMutexLock(&logMutex);

    /* Switch OFF the file logging */
    logRulePtr->log = mcsFALSE;

    mcsMutexUnlock(&logMutex);

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
mcsCOMPL_STAT logSetFileLogLevel(logLEVEL level)
{
    mcsMutexLock(&logMutex);

    /* Set the file logging level to the specified one */
    logRulePtr->logLevel = level;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Get the file logging level, as defined in the logLEVEL enumeration.
 *
 * \return current file logging level (verbosity), as defined in the logLEVEL
 * enumeration
 */
logLEVEL logGetFileLogLevel()
{
    logLEVEL level = logRulePtr->logLevel;

    /* Returns the file logging level */
    return (level);
}

/**
 * Switch stdout logging ON.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logEnableStdoutLog()
{
    mcsMutexLock(&logMutex);

    /* Switch ON the stdout logging */
    logRulePtr->verbose = mcsTRUE;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Switch stdout logging OFF.
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logDisableStdoutLog()
{
    mcsMutexLock(&logMutex);

    /* Switch OFF the stdout logging */
    logRulePtr->verbose = mcsFALSE;

    mcsMutexUnlock(&logMutex);

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
mcsCOMPL_STAT logSetStdoutLogLevel(logLEVEL level)
{
    mcsMutexLock(&logMutex);

    /* Set the stdout logging level to the specified one */
    logRulePtr->verboseLevel = level;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Get the stdout logging level, as defined in the logLEVEL enumeration.
 *
 * \return current stdout logging level (verbosity),  as defined in the logLEVEL
 * enumeration
 */
logLEVEL logGetStdoutLogLevel()
{
    logLEVEL level = logRulePtr->verboseLevel;

    /* Returns the stdout logging level */
    return (level);
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
    mcsMutexLock(&logMutex);

    /* Reset the number of allowed modules */
    logNbAllowedMod = 0;

    mcsMutexUnlock(&logMutex);

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
    mcsMutexLock(&logMutex);

    /* Check if table is full */
    if (logNbAllowedMod == logNB_MAX_ALLOWED_MOD)
    {
        UNLOCK_MUTEX_AND_RETURN_FAILURE();
    }

    /* Add module to the list */
    strncpy(logAllowedModList[logNbAllowedMod], mod, mcsMODULEID_LEN);
    logNbAllowedMod++;

    mcsMutexUnlock(&logMutex);

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
    mcsMutexLock(&logMutex);

    /* Set 'print date' flag  */
    logRulePtr->printDate = flag;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Return whether date output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if date printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintDate()
{
    mcsLOGICAL boolean = logRulePtr->printDate;

    return boolean;
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
    mcsMutexLock(&logMutex);

    /* Set 'print line/file' flag  */
    logRulePtr->printFileLine = flag;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Return whether fileline output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if fileline printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintFileLine()
{
    mcsLOGICAL boolean = logRulePtr->printFileLine;

    return boolean;
}

/**
 * Switch ON/OFF the process output (useful in test mode).
 *
 * \param flag mcsTRUE to turn process printing ON, mcsFALSE otherwise
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logSetPrintProcess(mcsLOGICAL flag)
{
    mcsMutexLock(&logMutex);

    /* Set 'print process' flag  */
    logRulePtr->printProcess = flag;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Return whether process output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if process printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintProcess()
{
    mcsLOGICAL boolean = logRulePtr->printProcess;

    return boolean;
}

/**
 * Switch ON/OFF the module output (useful in test mode).
 *
 * \param flag mcsTRUE to turn module printing ON, mcsFALSE otherwise
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logSetPrintModule(mcsLOGICAL flag)
{
    mcsMutexLock(&logMutex);

    /* Set 'print module' flag  */
    logRulePtr->printModule = flag;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Return whether module output is switched ON/OFF (useful in test mode).
 *
 * \return mcsTRUE if module printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintModule()
{
    mcsLOGICAL boolean = logRulePtr->printModule;

    return boolean;
}

/**
 * Switch ON/OFF the thread name output (useful in multi threading environment).
 *
 * \param flag mcsTRUE to turn thread name printing ON, mcsFALSE otherwise
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT logSetPrintThreadName(mcsLOGICAL flag)
{
    mcsMutexLock(&logMutex);

    /* Set 'print thread name' flag  */
    logRulePtr->printThreadName = flag;

    mcsMutexUnlock(&logMutex);

    return mcsSUCCESS;
}

/**
 * Return whether thread name output is switched ON/OFF (useful in multi threading environment).
 *
 * \return mcsTRUE if thread name printing is turned ON, mcsFALSE otherwise.
 */
mcsLOGICAL logGetPrintThreadName()
{
    mcsLOGICAL boolean = logRulePtr->printThreadName;

    return boolean;
}

/**
 * Log informations into file and stdout, according to the specified log level.
 *
 * \param modName name of the module relative.
 * \param level of message.
 * \param timeStamp (optional) time stamp of the message.
 * \param fileLine fileLine pointing source code of message.
 * \param logFormat format of given message.
 * \param argPtr (optional) argument list associated to the log message
 *
 * \return mcsCOMPL_STAT
 */
mcsCOMPL_STAT logPrint(const mcsMODULEID modName, const logLEVEL level, char* timeStamp,
                       const char* fileLine, const char* logFormat, ...)
{
    mcsCOMPL_STAT status = mcsSUCCESS;

    char buffer[BUFFER_MAX_LEN];
    buffer[0] = '\0';

    va_list argPtr;

    if (IS_NULL(timeStamp))
    {
        mcsSTRING32 infoTime;

        /* Get UNIX-style time and display as number and string. */
        logGetTimeStamp(infoTime);

        timeStamp = (char*) &infoTime;
    }

    /* If the log message should be file-logged, and that its log level is less
     * than or egal to the desired file-logging level
     */
    if (IS_TRUE(logRulePtr->log) && (level <= logRulePtr->logLevel))
    {
        /* Log information to file */
        va_start(argPtr, logFormat);
        vsnprintf(buffer, sizeof (buffer) - 1, logFormat, argPtr);
        va_end(argPtr);

        status = _logData(modName, level, timeStamp, fileLine, buffer);
    }

    /* If the log message should be stdout logged, and that its log level is
     * less than or egal to the desired stdout logging level
     */
    if (IS_TRUE(logRulePtr->verbose) && (level <= logRulePtr->verboseLevel))
    {
        /* Check if module belongs to the list of allowed modules */
        mcsLOGICAL allowed = mcsTRUE;
        if (logNbAllowedMod != 0)
        {
            int i;
            allowed = mcsFALSE;
            for (i = 0; (i < logNbAllowedMod) && IS_FALSE(allowed); i++)
            {
                if (strcmp(logAllowedModList[i], modName) == 0)
                {
                    allowed = mcsTRUE;
                }
            }
        }

        /* If message can be printed out */
        if (IS_TRUE(allowed))
        {
            const char* priorityMsg;

            /* initialize priority according given loglevel */
            switch (level)
            {
                case logERROR:      priorityMsg = "Error";
                    break;
                case logQUIET:      priorityMsg = "Quiet";
                    break;
                case logWARNING:    priorityMsg = "Warn ";
                    break;
                default:
                case logINFO:       priorityMsg = "Info " ;
                    break;
                case logTEST:       priorityMsg = "Test ";
                    break;
                case logDEBUG:	    priorityMsg = "Debug";
                    break;
                case logTRACE:      priorityMsg = "Trace";
                    break;
            }

            /* Note: 512 bytes is large enough to contain the complete prefix
             * No buffer overflow checks ! */
            mcsSTRING512 prefix;
            char*        prefixPtr = prefix;
            mcsSTRING256 tmp;

            prefix[0] = '\0';

            /* If the log message should contain the process */
            if (IS_TRUE(logRulePtr->printProcess))
            {
                sprintf(tmp, "%s - ", mcsGetProcName());
                prefixPtr = strcatFast(prefixPtr, tmp);
            }

            /* If the log message should contain the module */
            if (IS_TRUE(logRulePtr->printModule))
            {
                sprintf(tmp, "%6s - ", modName);
                prefixPtr = strcatFast(prefixPtr, tmp);
            }

            /* Print the log priority */
            sprintf(tmp, "%s - ", priorityMsg);
            prefixPtr = strcatFast(prefixPtr, tmp);

            /* If the log message should contain the date */
            if (IS_TRUE(logRulePtr->printDate))
            {
                sprintf(tmp, "%s - ", timeStamp);
                prefixPtr = strcatFast(prefixPtr, tmp);
            }

            /* If the log message should contain the thread name */
            if (IS_TRUE(logRulePtr->printThreadName))
            {
                /* Get the thread Name */
                mcsSTRING16 thName;
                mcsGetThreadName(&thName);

                sprintf(tmp, "%s - ", thName);
                prefixPtr = strcatFast(prefixPtr, tmp);
            }

            /* If the fileline exists and should be contained in the log message */
            if (IS_NOT_NULL(fileLine) && IS_TRUE(logRulePtr->printFileLine))
            {
                char* lastSlash = rindex(fileLine, '/');
                if (IS_NOT_NULL(lastSlash))
                {
                    sprintf(tmp, "%-28s - ", lastSlash + 1);
                }
                else
                {
                    sprintf(tmp, "%s - ", fileLine);
                }
                prefixPtr = strcatFast(prefixPtr, tmp);
            }

            /* Compute the variable parameters and print them */
            va_start(argPtr, logFormat);
            vsnprintf(buffer, BUFFER_MAX_LEN - 1, logFormat, argPtr);
            va_end(argPtr);

            fprintf(stdout, "%s%s\n", prefix, buffer);
            fflush(stdout);

            /* use log context ? */
            logTHREAD_CONTEXT *logContext = logGetThreadContext();

            if (IS_NOT_NULL(logContext) && IS_TRUE(logContext->enabled))
            {
                /* append log message into log context */
                logDynBufAppendLine(logContext, prefix);
                logDynBufAppendString(logContext, buffer);
            }
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
mcsCOMPL_STAT _logData(const mcsMODULEID modName, logLEVEL level,
                       const char *timeStamp, const char *fileLine,
                       const char *logText)
{
    /* Message formating gstuff */
    char           *priorityMsg = NULL;
    char            logMsg[BUFFER_MAX_LEN];
    logMsg[0]       = '\0';

    /* initialize priority according given loglevel */
    switch (level)
    {
        case logERROR:      priorityMsg = "Error";
            break;
        case logQUIET:      priorityMsg = "Quiet";
            break;
        case logWARNING:    priorityMsg = "Warn ";
            break;
        default:
        case logINFO:       priorityMsg = "Info " ;
            break;
        case logTEST:       priorityMsg = "Test ";
            break;
        case logDEBUG:	    priorityMsg = "Debug";
            break;
        case logTRACE:      priorityMsg = "Trace";
            break;
    }

    /* Get the thread Name */
    mcsSTRING16 thName = "Main";

    /* If the log message should contain the thread name */
    if (IS_TRUE(logRulePtr->printThreadName))
    {
        /* Get the thread Name */
        mcsGetThreadName(&thName);
    }

    /* Compute the log message */
    snprintf(logMsg, sizeof (logMsg) - 1,  "%s - %s - %6s - %s - %s - %s - %s - %s", mcsGetEnvName(),
             mcsGetProcName(), modName, priorityMsg, timeStamp, thName, fileLine, logText);

    mcsMutexLock(&logMutex);

    /* If no specific logManager host name has not been redefined by the log
     * library user...
     */
    mcsUINT32 logManagerHostNameLength = strlen(logRulePtr->logManagerHostName);

    if (logManagerHostNameLength == 0)
    {
        /* Try to get the local host name */
        if (logGetHostName(logRulePtr->logManagerHostName,
                           sizeof (logRulePtr->logManagerHostName)) == mcsFAILURE)
        {
            mcsSTRING1024 errorMsg;
            logPrintErrMessage("- LOG LIBRARY ERROR - logGetHostName() failed - %s", mcsStrError(errno, errorMsg));
            UNLOCK_MUTEX_AND_RETURN_FAILURE();
        }

        /* If the local host name seems empty... */
        if (IS_NULL(logRulePtr->logManagerHostName)
                || (strlen(logRulePtr->logManagerHostName) == 0))
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - got an empty hostname");
            UNLOCK_MUTEX_AND_RETURN_FAILURE();
        }
    }

    /* If the connection to the logManager is NOT already opened, open it */
    if (IS_FALSE(logSocketIsAlreadyOpen))
    {
        /* Try to create ths socket */
        sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock == -1)
        {
            mcsSTRING1024 errorMsg;
            logPrintErrMessage("- LOG LIBRARY ERROR - socket() failed - %s", mcsStrError(errno, errorMsg));
            UNLOCK_MUTEX_AND_RETURN_FAILURE();
        }

        /* NOTE: posix unthread safe function gethostbyname() */
        struct hostent *hp = gethostbyname(logRulePtr->logManagerHostName);
        if (IS_NULL(hp))
        {
            logPrintErrMessage("- LOG LIBRARY ERROR - gethostbyname(%s) failed", logRulePtr->logManagerHostName);
            UNLOCK_MUTEX_AND_RETURN_FAILURE();
        }

        /* Copy the resolved information into the sockaddr_in structure */
        memset(&server, '\0', sizeof (server));
        memcpy(&(server.sin_addr), hp->h_addr, hp->h_length);
        server.sin_family = hp->h_addrtype;
        server.sin_port   = htons(logRulePtr->logManagerPortNumber);

        logSocketIsAlreadyOpen = mcsTRUE;

        logPrintErrMessage("- log - socket initialized to '%s'", logRulePtr->logManagerHostName);
    }

    mcsMutexUnlock(&logMutex);

    /* Send message to the logManager process */
    if (sendto(sock, (void *) logMsg, strlen(logMsg), MSG_NOSIGNAL,
               (const struct sockaddr *) &server, sizeof (server)) == -1)
    {
        mcsSTRING1024 errorMsg;
        logPrintErrMessage("- LOG LIBRARY ERROR - sendto() failed - %s", mcsStrError(errno, errorMsg));
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
void logGetTimeStamp(mcsSTRING32 timeStamp)
{
    struct timeval time;
    struct tm      timeNow;
    mcsSTRING12    tmpBuf;

    /* Get local time */
    gettimeofday(&time, NULL);

    /* Format the date */
    gmtime_r(&time.tv_sec, &timeNow);
    strftime(timeStamp, sizeof (mcsSTRING32) - 1, "%Y-%m-%dT%H:%M:%S", &timeNow);

    /* Add milli-second and micro-second */
    snprintf(tmpBuf, sizeof (mcsSTRING12) - 1, "%.6f", time.tv_usec / 1e6);
    strncat(timeStamp, &tmpBuf[1], sizeof (mcsSTRING32) - 1);
}


/* --- Thread Local Storage handling for log context */

/**
 * Enable the log context per thread
 * \return mcsSUCCESS or mcsFAILURE if the thread local storage is not initialized.
 */
mcsCOMPL_STAT logEnableThreadContext(void)
{
    logTHREAD_CONTEXT *logContext = logGetThreadContext();

    if (IS_NOT_NULL(logContext))
    {
        logContext->enabled = mcsTRUE;
        return mcsSUCCESS;
    }
    return mcsFAILURE;
}

/**
 * Return the internal buffer of the log context and disable the log context
 * @return internal buffer of the log context or NULL if the log context is disabled.
 */
const char* logContextGetBuffer(void)
{
    logTHREAD_CONTEXT *logContext = logGetThreadContext();

    if (IS_NOT_NULL(logContext))
    {
        const char* dynBuf = logContext->dynBuf;

        /* disable log context */
        logContext->enabled = mcsFALSE;

        return dynBuf;
    }
    return NULL;
}

/**
 * Get the log context relative to the current pthread
 * \return pointer on the log context struct or NULL if undefined.
 */
logTHREAD_CONTEXT* logGetThreadContext()
{
    /* NOTE: no log statements in this method to avoid recursive loop */

    if (IS_FALSE(logInitialized))
    {
        return NULL;
    }

    void* global;
    logTHREAD_CONTEXT* logContext;

    global = pthread_getspecific(tlsKey_logContext);

    if (IS_NULL(global))
    {
        /* first time - create the log context */
        logContext = (logTHREAD_CONTEXT*) malloc(sizeof (logTHREAD_CONTEXT));

        /* Initialize the log context */
        logContext->enabled        = mcsFALSE; /* disabled by default */
        logContext->dynBuf         = NULL;
        logContext->storedBytes    = 0;
        logContext->allocatedBytes = 0;

        pthread_setspecific(tlsKey_logContext, logContext);
    }
    else
    {
        logContext = (logTHREAD_CONTEXT*) global;
    }

    return logContext;
}

/**
 * Thread local key destructor
 * @param value value to free
 */
static void tlsLogContextDestructor(void* value)
{
    logTHREAD_CONTEXT* logContext;
    logContext = (logTHREAD_CONTEXT*) value;

    if (IS_TRUE(logTHREAD_CONTEXT_DUMP) && (logContext->storedBytes != 0))
    {
        /* DEBUG */
        fprintf(stdout, "\n<DUMP TLS Logs>\n%s\n</DUMP TLS Logs>\n\n", logContext->dynBuf);
        fflush(stdout);
    }

    if (IS_TRUE(logTHREAD_LOG_BUFFER_SIZE))
    {
        printf("logDynBuf(destroy): %u reserved; %u stored\n", logContext->allocatedBytes, logContext->storedBytes);
    }

    /* free dynamic buffer */
    logDynBufDestroy(logContext);

    /* free values */
    free(value);
    pthread_setspecific(tlsKey_logContext, NULL);
}

/**
 * Initialize the thread local storage for log thread context
 */
mcsCOMPL_STAT logInit(void)
{
    logDebug("logInit:  enable log thread context support");

    const int rc = pthread_key_create(&tlsKey_logContext, tlsLogContextDestructor);
    if (rc != 0)
    {
        return mcsFAILURE;
    }
    logInitialized = mcsTRUE;
    return mcsSUCCESS;
}

/**
 * Destroy the thread local storage for log thread context
 */
mcsCOMPL_STAT logExit(void)
{
    logDebug("logExit: disable log thread context support");

    /* Get and free main error stack */
    logTHREAD_CONTEXT *logContext = logGetThreadContext();
    tlsLogContextDestructor(logContext);

    pthread_key_delete(tlsKey_logContext);

    logInitialized = mcsFALSE;

    return mcsSUCCESS;
}


/* log dynamic buffer handling */
/**
 * Dynamic Buffer first position number abstraction.
 *
 * It is meant to make independant all the code from the number internally used
 * to reference the first byte of a Dynamic Buffer, in order to make your work
 * independant of our futur hypotetic implementation changes.
 */
#define logDYN_BUF_BEGINNING_POSITION ((mcsUINT32) 1u)

/** minimum buffer capacity */
#define logDYN_BUF_MIN_CAPACITY       ((mcsINT32) 128 * 1024)

/** minimum buffer resize capacity */
#define logDYN_BUF_MIN_EXTEND         ((mcsINT32)  64 * 1024)

/**
 * Verify if a received string (a null terminated char array) is valid or not.
 *
 * @param str address of the string, already allocated extern buffer
 *
 * @return string length, or 0 if it is not valid
 */
static mcsUINT32 logDynBufChkStringParam(const char *str)
{
    /* Test the 'str' parameter validity */
    if (str == NULL)
    {
        return 0;
    }

    /*
     * Return the number of bytes stored in the received string including its
     * ending '\0'
     */
    return (strlen(str) + 1);
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'position' is correct (eg. inside the Dynamic Buffer range).
 *
 * This function is only used internally by funtions receiving 'position'
 * parameter.
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param position a position inside the Dynamic Buffer
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT logDynBufChkPositionParam(const logTHREAD_CONTEXT *dynBuf,
                                               const mcsUINT32          position)
{
    /* Test the position parameter validity... */
    if (position < logDYN_BUF_BEGINNING_POSITION ||
            position > dynBuf->storedBytes)
    {
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Verify if a Dynamic Buffer has already been initialized, and if the given
 * 'from' and 'to' position are correct (eg. inside the Dynamic Buffer range,
 * and 'from' parameterlower than 'to' parameter).
 *
 * This function is only used internally by funtions receiving 'from' and 'to'
 * parameters.
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param from a position inside the Dynamic Buffer
 * @param to a position inside the Dynamic Buffer
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT logDynBufChkFromToParams(const logTHREAD_CONTEXT *dynBuf,
                                              const mcsUINT32          from,
                                              const mcsUINT32          to)
{
    /* Test the 'from' parameter validity */
    if (from < logDYN_BUF_BEGINNING_POSITION || from > dynBuf->storedBytes)
    {
        return mcsFAILURE;
    }

    /* Test the 'to' parameter validity */
    if ((to < logDYN_BUF_BEGINNING_POSITION) || (to > dynBuf->storedBytes))
    {
        return mcsFAILURE;
    }

    /* Test the 'from' and 'to' parameters validity against each other */
    if (to < from)
    {
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Give back a Dynamic Buffer byte stored at a given position.
 *
 * @warning The first Dynamic Buffer byte has the position value defined by the
 * logDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param byte address of the receiving, already allocated extern byte that will
 * hold the seeked Dynamic Buffer byte
 * @param position position of the Dynamic Buffer seeked byte
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufGetByteAt(const logTHREAD_CONTEXT *dynBuf,
                                 char                    *byte,
                                 const mcsUINT32          position)
{
    /* Test the 'dynBuf' and 'position' parameters validity */
    FAIL(logDynBufChkPositionParam(dynBuf, position));

    /* Test the 'write to' byte buffer address parameter validity */
    FAIL_NULL(byte);

    /* Write back the seeked character inside the byte buffer parameter */
    *byte = dynBuf->dynBuf[position - logDYN_BUF_BEGINNING_POSITION];

    return mcsSUCCESS;
}

/**
 * Delete a given range of a Dynamic Buffer bytes.
 *
 * @warning The first Dynamic Buffer byte has the position value defined by the
 * logDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param from position of the first Dynamic Buffer byte to be deleted
 * @param to position of the last Dynamic Buffer byte to be deleted
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufDeleteBytesFromTo(logTHREAD_CONTEXT    *dynBuf,
                                         const mcsUINT32       from,
                                         const mcsUINT32       to)
{
    /* Test the 'dynBuf', 'from' and 'to' parameters validity */
    FAIL(logDynBufChkFromToParams(dynBuf, from, to));

    const mcsUINT32 storedBytes = dynBuf->storedBytes;

    /* special case to remove last byte */
    if ((from == to) && (to == storedBytes))
    {
        dynBuf->storedBytes -= 1;
        return mcsSUCCESS;
    }

    /* Compute the number of Dynamic Buffer bytes to be backed up */
    mcsINT32 lengthToBackup = storedBytes -
            ((to - logDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be backep up' Dynamic Buffer byte position */
    char *positionToBackup = dynBuf->dynBuf +
            ((to - logDYN_BUF_BEGINNING_POSITION) + 1);

    /* Compute the first 'to be deleted' Dynamic Buffer byte position */
    char *positionToWriteIn = dynBuf->dynBuf +
            (from - logDYN_BUF_BEGINNING_POSITION);

    /*
     * Move the 'not-to-be-deleted' Dynamic Buffer bytes to their
     * definitive place
     */
    memmove(positionToWriteIn, positionToBackup, lengthToBackup);

    /*
     * Update the Dynamic Buffer stored length value using the deleted bytes
     * number
     */
    dynBuf->storedBytes -= ((to - logDYN_BUF_BEGINNING_POSITION) -
            (from - logDYN_BUF_BEGINNING_POSITION)) + 1;

    return mcsSUCCESS;
}

/**
 * Smartly allocate and add a number of bytes to a Dynamic Buffer.
 *
 * If the Dynamic Buffer already has some allocated bytes, its length is
 * automatically expanded, with the previous content remaining untouched.
 *
 * Newly allocated bytes will all be set to '0'.
 *
 * @remark The call to this function is optional, as a Dynamic Buffer will
 * expand itself on demand when invoquing other logDynBuf functions as
 * logDynBufAppendBytes(), logDynBufInsertBytesAt(), etc...
 * So, this function call is only usefull when you know by advance the maximum
 * bytes length the Dynamic Buffer could reach accross its entire life, and thus
 * want to minimize the CPU time spent to expand the Dynamic Buffer
 * allocated memory on demand.\n\n
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param length number of bytes by which the Dynamic Buffer should be expanded
 * (if less than or equal to 0, nothing is done).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufAlloc(logTHREAD_CONTEXT *dynBuf,
                             const mcsINT32     length)
{
    /* If the current buffer already has sufficient length... */
    if (length <= 0)
    {
        /* Do nothing */
        return mcsSUCCESS;
    }

    char *newBuf = NULL;

    /* new total size */
    mcsINT32 newAllocSize = 0;
    mcsINT32 minNewSize = 0;

    /* If the buffer has no memory allocated... */
    if (dynBuf->allocatedBytes == 0)
    {
        /* allocate at least min_capacity bytes */
        newAllocSize = mcsMAX(length, logDYN_BUF_MIN_CAPACITY);

        /* Allocate the desired length */
        FAIL_NULL((dynBuf->dynBuf = calloc(newAllocSize, sizeof (char))));
    }
    else
    {
        /* The buffer needs to be expanded : Get more memory */
        minNewSize = dynBuf->allocatedBytes + length;

        /* reallocate at least min_extend bytes or needed length */
        newAllocSize = mcsMAX(minNewSize, logDYN_BUF_MIN_EXTEND);

        /* enlarge buffer enough to avoid excessive reallocation */
        if (minNewSize < 4 * 1024 * 1024) /* 4Mb */
        {
            /* grow by x2 */
            newAllocSize = mcsMAX(newAllocSize, minNewSize * 2);
        }
        else if (minNewSize < 64 * 1024 * 1024) /* 64Mb */
        {
            /* grow by x1.5 */
            newAllocSize = mcsMAX(newAllocSize, (minNewSize * 3) / 2);
        }
        else
        {
            /* grow by 10% */
            newAllocSize = mcsMAX(newAllocSize, minNewSize + minNewSize / 10);
        }

        if (IS_TRUE(logTHREAD_LOG_BUFFER_SIZE))
        {
            printf("logDynBuf(realloc): %d reserved; %d needed\n", newAllocSize, minNewSize);
        }

        FAIL_NULL((newBuf = realloc(dynBuf->dynBuf, newAllocSize)));

        /* Store the expanded buffer address */
        dynBuf->dynBuf = newBuf;
    }

    /* Set the buffer allocated length value */
    dynBuf->allocatedBytes = newAllocSize;

    return mcsSUCCESS;
}

mcsCOMPL_STAT logDynBufReserve(logTHREAD_CONTEXT *dynBuf,
                               const mcsINT32     length)
{
    /* Expand the received Dynamic Buffer size */
    mcsINT32 bytesToAlloc;
    bytesToAlloc = length - (dynBuf->allocatedBytes - dynBuf->storedBytes);

    /* If the current buffer already has sufficient length... */
    if (bytesToAlloc <= 0)
    {
        /* Do nothing */
        return mcsSUCCESS;
    }
    return logDynBufAlloc(dynBuf, bytesToAlloc);
}

/**
 * Destroy a Dynamic Buffer.
 *
 * Possibly allocated memory is freed and zeroed - so be sure that it is
 * desirable to delete the data contained inside the buffer.
 *
 * @warning A Dynamic Buffer <b> must be destroyed </b> after use.\n\n
 *
 * @param dynBuf address of a Dynamic Buffer structure
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufDestroy(logTHREAD_CONTEXT *dynBuf)
{
    /* If some memory was allocated... */
    if (dynBuf->allocatedBytes != 0)
    {
        /* Free the allocated memory */
        free(dynBuf->dynBuf);
    }

    return mcsSUCCESS;
}

/**
 * Replace a Dynamic Buffer byte at a given position.
 *
 * @warning The first Dynamic Buffer byte has the position value defined by the
 * miscDYN_BUF_BEGINNING_POSITION macro.\n\n
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param byte byte to be written in the Dynamic Buffer
 * @param position position of the Dynamic Buffer byte to be over-written
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufReplaceByteAt(logTHREAD_CONTEXT *dynBuf,
                                     const char        byte,
                                     const mcsUINT32   position)
{
    /* Test the 'dynBuf' and 'position' parameters validity */
    FAIL(logDynBufChkPositionParam(dynBuf, position));

    /* Overwrite the specified Dynamic Buffer byte with the received one */
    dynBuf->dynBuf[position - logDYN_BUF_BEGINNING_POSITION] = byte;

    return mcsSUCCESS;
}

/**
 * Append a given bytes buffer contnent at the end of a Dynamic Buffer.
 *
 * Copy all the extern buffer bytes content at the end of the given Dynamic
 * Buffer.
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param bytes address of the extern buffer bytes to be written at the end of
 * the Dynamic Buffer
 * @param length number of extern buffer bytes to be written at the end of the
 * Dynamic Buffer
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufAppendBytes(logTHREAD_CONTEXT    *dynBuf,
                                   const char           *bytes,
                                   const mcsUINT32       length)
{
    /* If nothing to append */
    if (length <= 0)
    {
        /* Return immediately */
        return mcsSUCCESS;
    }

    /* Test the 'bytes' parameter validity */
    FAIL_NULL(bytes);

    /* Expand the received Dynamic Buffer size */
    FAIL(logDynBufReserve(dynBuf, length));

    /* Copy the extern buffer bytes at the end of the Dynamic Buffer */
    memcpy(dynBuf->dynBuf + dynBuf->storedBytes, bytes, length);

    /*
     * Update the Dynamic Buffer stored length value using the number of the
     * extern buffer bytes
     */
    dynBuf->storedBytes += length;

    return mcsSUCCESS;
}

/**
 * Append a given string content at the end of a Dynamic Buffer.
 *
 * Copy a null-terminated extern string content at the end of a Dynamic Buffer,
 * adding an '\\0' at the end of it.
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param str address of the extern string content to be written at the end of
 * the Dynamic Buffer
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufAppendString(logTHREAD_CONTEXT *dynBuf,
                                    const char        *str)
{
    /* Test the 'str' parameter validity */
    mcsUINT32 stringLength;
    stringLength = logDynBufChkStringParam(str);
    if (stringLength == 0)
    {
        return mcsFAILURE;
    }

    /* Get the Dynamic Buffer stored bytes number */
    mcsUINT32 storedBytes = dynBuf->storedBytes;

    /* If the Dynamic Buffer already contain something... */
    if (storedBytes != 0)
    {
        /* Get the last character of the Dynamic Buffer */
        char lastDynBufChr = '\0';
        FAIL(logDynBufGetByteAt(dynBuf, &lastDynBufChr, storedBytes));

        /*
         * If the Dynamic Buffer was already holding a null-terminated string...
         */
        if (lastDynBufChr == '\0')
        {
            /* Remove the ending '\0' from the Dynamic Buffer */
            FAIL(logDynBufDeleteBytesFromTo(dynBuf, storedBytes, storedBytes));
        }
    }

    /* Append the string bytes, including its '\0' */
    return (logDynBufAppendBytes(dynBuf, str, stringLength));
}

/**
 * Append a given line at the end of a Dynamic Buffer.
 *
 * Copy a carriage return ('\\n') followed by an extern null-terminated string
 * at the end of a Dynamic Buffer, adding an '\\0' at the end of it.
 *
 * @param dynBuf address of a Dynamic Buffer structure
 * @param line address of the extern string to be written at the end of a
 * Dynamic Buffer
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT logDynBufAppendLine(logTHREAD_CONTEXT *dynBuf,
                                  const char       *line)
{
    /* Test the 'line' parameter validity */
    mcsUINT32 lineLength;
    lineLength = logDynBufChkStringParam(line);
    if (lineLength == 0)
    {
        return mcsFAILURE;
    }

    /* Get the Dynamic Buffer stored bytes number */
    mcsUINT32 storedBytes = dynBuf->storedBytes;

    /* If the Dynamic Buffer already contain something... */
    if (storedBytes != 0)
    {

        /* Get the last character of the Dynamic Buffer */
        char lastDynBufChr = '\0';
        FAIL(logDynBufGetByteAt(dynBuf, &lastDynBufChr, storedBytes));

        /*
         * If the Dynamic Buffer was already holding a null-terminated string...
         */
        if (lastDynBufChr == '\0')
        {
            /* Replace the ending '\0' by an '\n' */
            FAIL(logDynBufReplaceByteAt(dynBuf, '\n', storedBytes));
        }
    }

    /* Append the line, with its '\0' */
    return (logDynBufAppendBytes(dynBuf, line, lineLength));
}


/*___oOo___*/
