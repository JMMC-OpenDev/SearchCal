#ifndef log_H
#define log_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: log.h,v 1.21 2011-02-10 16:31:59 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.20  2005/09/05 14:35:17  gzins
* Added logEXTDBG definition for backward-compatibility
*
* Revision 1.19  2005/06/01 13:19:16  gzins
* Changed 'extended debug' to 'trace'
*
* Revision 1.18  2005/01/26 17:27:47  lafrasse
* Added automatic CVS history, refined user documentation, removed all
* ActionLog-related code, and changed SUCCESS in mcsSUCCESS and FAILURE in
* mcsFAILURE
*
* gzins     20-Dec-2004  Added functions to filter stdout log depending on
*                        module name
*
* gzins     18-Nov-2004  Added logError macro
*
* lafrasse  10-Aug-2004  Moved logGetTimeStamp back in log.h
*                        Changed back to logData original API
*
* lafrasse  03-Aug-2004  Changed logData API
*                        Moved local functions logGetTimeStamp declaration to
*                        logPrivate.h
*                        Added logSetLogManagerHostName and
*                        logSetLogManagerPortNumber functions
*
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
*
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
* mella     07-May-2004  Created
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
* Main header file, holding all the public APIs of this module.
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
    logTRACE
} logLEVEL;


/*
 * Pubic functions declaration
 */

/*
 * File and Stdout logging functions
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

mcsCOMPL_STAT logClearStdoutLogAllowedModList(void);
mcsCOMPL_STAT logAddToStdoutLogAllowedModList(char*);


mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL);
mcsLOGICAL logGetPrintDate(void);
mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL);
mcsLOGICAL logGetPrintFileLine(void);


mcsCOMPL_STAT logPrint(const mcsMODULEID, logLEVEL, const char *, const char *,
                       ...);
mcsCOMPL_STAT logData(const mcsMODULEID, logLEVEL, const char *, const char *,
                      const char *logText);

void          logGetTimeStamp(mcsBYTES32);


/*
 * Convenience macros
 */

/**
 * Log informations about errors (to the least detailed log level).
 *
 * All informations given to this macro are logged on the logERROR level, and
 * all the more detailed levels.
 */
#define logError(format, arg...) \
        logPrint(MODULE_ID, logERROR, __FILE_LINE__, format, ##arg)

/**
 * Log informations about abnormal events.
 *
 * All informations given to this macro are logged on the logWARNING level, and
 * all the more detailed levels.
 */
#define logWarning(format, arg...) \
        logPrint(MODULE_ID, logWARNING, __FILE_LINE__, format, ##arg)

/** 
 * Log informations about major events (eg when operational mode is modified).
 *
 * All informations given to this macro are logged on the logINFO level, and
 * all the more detailed levels.
 */
#define logInfo(format, arg...) \
        logPrint(MODULE_ID, logINFO, __FILE_LINE__, format, ##arg)

/** 
 * Log relevant informations used for software test activities.
 *
 * All informations given to this macro are logged on the logTEST level, and
 * all the more detailed levels.
 */
#define logTest(format, arg...) \
        logPrint(MODULE_ID, logTEST, __FILE_LINE__, format, ##arg)

/**
 * Log debugging informations.
 *
 * All informations given to this macro are logged on the logDEBUG level, and
 * all the more detailed levels.
 */
#define logDebug(format, arg...) \
        logPrint(MODULE_ID, logDEBUG, __FILE_LINE__, format, ##arg)

/**
 * Log function/method trace.
 *
 * This level is dedicated to add the name of each called function to the
 * logTRACE level log, in order to effectively trace each function call.
 *
 * This macro \em must be the first call (i.e. at the beginning) of each
 * function, in order to log their name.
 *
 * All informations given to this macro are logged on the logTRACE level.
 */
#define logTrace(format, arg...) \
    logPrint(MODULE_ID, logTRACE, __FILE_LINE__, format, ##arg)

/* OBSSOLETE - Keep for backward-compatibility */
#define logExtDbg logTrace
#define logEXTDBG logTRACE


#ifdef __cplusplus
};
#endif

  
#endif /*!log_H*/


/*___oOo___*/
