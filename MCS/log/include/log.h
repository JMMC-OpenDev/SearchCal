#ifndef log_H
#define log_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: log.h,v 1.12 2004-07-30 14:34:58 lafrasse Exp $"
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

typedef enum {
    logERROR = -1,
    logQUIET,
    logWARNING,
    logINFO,
    logTEST,
    logDEBUG,
    logEXTDBG
} logLEVEL;

/*
 * Log/Verbose/Action Logging Functions
 */
mcsCOMPL_STAT logPrint(const mcsMODULEID modName, logLEVEL level,
                       const char *fileLine, 
                       const char *logText, ...);

mcsCOMPL_STAT logPrintAction(logLEVEL level,
                             const char *logText, ...);


mcsCOMPL_STAT logSetFileLogState(mcsLOGICAL flag);

mcsCOMPL_STAT logSetFileLogVerbosity(logLEVEL level);

logLEVEL      logGetFileLogVerbosity(void);


mcsCOMPL_STAT logSetStdoutLogState(mcsLOGICAL flag);

mcsCOMPL_STAT logSetStdoutLogVerbosity(logLEVEL level);

logLEVEL      logGetStdoutLogVerbosity(void);


mcsCOMPL_STAT logSetActionLogVerbosity(logLEVEL level);

logLEVEL      logGetActionLogVerbosity(void);


mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL flag);

mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL flag);

mcsCOMPL_STAT logData(const mcsMODULEID modName, 
                      logLEVEL level,
                      const char *timeStamp,
                      const char *fileLine,
                      const char *buffer);

void logGetTimeStamp(mcsBYTES32 timeStamp);

/*
 * Convenience macros
 */

/* Logging and Verbose */

/**
 * Log information about errors or abnormal events for application. The
 * logging level is fixed to logWARNING.
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

/* Action logs */
    
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
