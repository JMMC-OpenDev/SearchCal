#ifndef log_H
#define log_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: log.h,v 1.17 2004-12-20 13:24:41 gzins Exp $"
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* mella     07-May-2004  Created
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
* lafrasse  03-Aug-2004  Changed logData API
*                        Moved local functions logGetTimeStamp declaration to
*                        logPrivate.h
*                        Added logSetLogManagerHostName and
*                        logSetLogManagerPortNumber functions
* lafrasse  10-Aug-2004  Moved logGetTimeStamp back in log.h
*                        Changed back to logData original API
* gzins     18-Nov-2004  Added logError macro
* gzins     20-Dec-2004  Added functions to filter stdout log depending on
*                        module name
*
*
*******************************************************************************/

/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/** \file
* Main header file.
*/


/*
* MCS Headers
*/
#include "mcs.h"


/*
 * Logging level constants
 */
typedef enum
{
    logERROR = -1,
    logQUIET,
    logWARNING,
    logINFO,
    logTEST,
    logDEBUG,
    logEXTDBG
} logLEVEL;


/*
 * Pubic functions declaration
 */

/*
 * File/Stdout/Action Logging Functions
 */
mcsCOMPL_STAT logSetLogManagerHostName(mcsBYTES256);
mcsCOMPL_STAT logSetLogManagerPortNumber(mcsUINT32);


mcsCOMPL_STAT logEnableFileLog(void);
mcsCOMPL_STAT logDisableFileLog(void);
mcsCOMPL_STAT logSetFileLogLevel(logLEVEL);
logLEVEL      logGetFileLogLevel(void);


mcsCOMPL_STAT logEnableStdoutLog(void);
mcsCOMPL_STAT logDisableStdoutLog(void);
mcsCOMPL_STAT logSetStdoutLogLevel(logLEVEL);
logLEVEL      logGetStdoutLogLevel(void);

void logClearStdoutLogAllowedModList(void);
mcsCOMPL_STAT logAddToStdoutLogAllowedModList(char*);

mcsCOMPL_STAT logSetActionLogLevel(logLEVEL);
logLEVEL      logGetActionLogLevel(void);


mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL);
mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL);


mcsCOMPL_STAT logPrint(const mcsMODULEID, logLEVEL, const char *, const char *,
                       ...);
mcsCOMPL_STAT logPrintAction(logLEVEL, const char *, ...);
mcsCOMPL_STAT logData(const mcsMODULEID, logLEVEL, const char *, const char *,
                      const char *logText);

void          logGetTimeStamp(mcsBYTES32);


/*
 * Convenience macros
 */

/* File and Stdout related */

/**
 * Log information about errors for application. The logging level is fixed to
 * logERROR.
 */
#define logError(format, arg...) \
    logPrint(MODULE_ID, logERROR, __FILE_LINE__, format, ##arg)

/**
 * Log information about abnormal events for application. The logging level is
 * fixed to logWARNING.
 */
#define logWarning(format, arg...) \
    logPrint(MODULE_ID, logWARNING, __FILE_LINE__, format, ##arg)

/** 
 * Log information about major events. For example, when operational mode is
 * modified. The logging level is fixed to logINFO.
 */
#define logInfo(format, arg...) \
    logPrint(MODULE_ID, logINFO, __FILE_LINE__, format, ##arg)

/** 
 * Log relevant information used for the software test activities. The logging
 * level is fixed to logTEST.
 */
#define logTest(format, arg...) \
    logPrint(MODULE_ID, logTEST, __FILE_LINE__, format, ##arg)

/**
 * Log debugging information. The logging level is fixed to logDEBUG.
 */
#define logDebug(format, arg...) \
    logPrint(MODULE_ID, logDEBUG, __FILE_LINE__, format, ##arg)

/**
 * Log more detailed debugging information. This level is dedicated to log the
 * name of the called function/method; i.e. it is used at the beginning of
 * function/method to log the name of this function/method. The logging level
 * is fixed to logEXTDBG.
 */
#define logExtDbg(format, arg...) \
    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, format, ##arg)

/* Action related */
    
#define logAction(level, format, arg...) \
    logPrintAction(level, format, ##arg); \
    logPrint(MODULE_ID, level, __FILE_LINE__, format, ##arg)

#define logWarningAction(format, arg...) \
    logPrintAction(logWARNING, format, ##arg); \
    logPrint(MODULE_ID, logWARNING, __FILE_LINE__, format, ##arg)

#define logInfoAction(format, arg...) \
    logPrintAction(logINFO, format, ##arg); \
    logPrint(MODULE_ID, logINFO, __FILE_LINE__, format, ##arg)

#define logTestAction(format, arg...) \
    logPrintAction(logTEST, format, ##arg); \
    logPrint(MODULE_ID, logTEST, __FILE_LINE__, format, ##arg)

#define logDebugAction(format, arg...) \
    logPrintAction(logDEBUG, format, ##arg); \
    logPrint(MODULE_ID, logDEBUG, __FILE_LINE__, format, ##arg)

#define logExtDbgAction(format, arg...) \
    logPrintAction(logEXTDBG, format, ##arg); \
    logPrint(MODULE_ID, logEXTDBG, __FILE_LINE__, format, ##arg)


#ifdef __cplusplus
};
#endif

  
#endif /*!log_H*/


/*___oOo___*/
