#ifndef logLog_H
#define logLog_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * MCS Headers
 */
#include "mcs.h"

/* Macro */
    
#ifndef __cplusplus
#ifndef __FILE_LINE__
#define logIToStr(a) #a
#define logIToStr2(a) logIToStr(a) 
#define __FILE_LINE__ __FILE__ ":" logIToStr2(__LINE__)
#endif /*!__FILE_LINE__*/
#endif

/* 
 * Log Constants 
 */
#define logTEXT_LEN 64
    
/*
 * Logging level constants
 */

typedef enum {
    logQUIET,
    logWARNING,
    logINFO,
    logTEST,
    logDEBUG,
    logEXTDBG
} logLEVEL;

/*
 * Define logging definition structure 
 */
typedef struct {
        mcsLOGICAL  log;
        mcsLOGICAL  verbose;
        logLEVEL    logLevel;
        logLEVEL    verboseLevel;
        logLEVEL    actionLevel;
        mcsLOGICAL  printDate;
        mcsLOGICAL  printFileLine;
} logRULE;

/*
 * Log/Verbose/Action Logging Functions
 */
extern mcsCOMPL_STAT logIdentify(const mcsPROCNAME processName, 
                                 const mcsMODULEID moduleName);

extern mcsCOMPL_STAT logPrint(logLEVEL level, const char *fileLine, 
                              const char *logText, ...);

extern mcsCOMPL_STAT logPrintAction(logLEVEL level,
                                    const char *logText, ...);



extern mcsCOMPL_STAT logSetLog(mcsLOGICAL flag);
    
extern mcsCOMPL_STAT logSetLogLevel(logLEVEL level);

extern logLEVEL      logGetLogLevel(void);



extern mcsCOMPL_STAT logSetVerbose(mcsLOGICAL flag);

extern mcsCOMPL_STAT logSetVerboseLevel(logLEVEL level);

extern logLEVEL      logGetVerboseLevel(void);



extern mcsCOMPL_STAT logSetActionLevel(logLEVEL level);

extern logLEVEL      logGetActionLevel(void);



extern mcsCOMPL_STAT logSetPrintDate(mcsLOGICAL flag);

extern mcsCOMPL_STAT logSetPrintFileLine(mcsLOGICAL flag);


/* should be placed in a private area */
extern mcsCOMPL_STAT logData(const char * msg);

/*
 * Convenience macros
 */

/* Logging and Verbose */

#define logWarning(format, arg...) \
    logPrint( logWARNING, __FILE_LINE__, format, ##arg)

#define logInfo(format, arg...) \
    logPrint( logINFO, __FILE_LINE__, format, ##arg)

#define logTest(format, arg...) \
    logPrint( logTEST, __FILE_LINE__, format, ##arg)

#define logDebug(format, arg...) \
    logPrint( logDEBUG, __FILE_LINE__, format, ##arg)

#define logExtDbg(format, arg...) \
    logPrint( logEXTDBG, __FILE_LINE__, format, ##arg)

/* Action logs */
    
#define logAction(level, format, arg...) \
    logPrintAction(level, format, ##arg); \
    logPrint(level, __FILE_LINE__, format, ##arg)

#define logWarningAction(format, arg...) \
    logPrintAction(logWARNING, format, ##arg); \
    logPrint(logWARNING, __FILE_LINE__, format, ##arg)

#define logInfoAction(format, arg...) \
    logPrintAction(logINFO, format, ##arg); \
    logPrint(logINFO, __FILE_LINE__, format, ##arg)

#define logTestAction(format, arg...) \
    logPrintAction(logTEST, format, ##arg); \
    logPrint(logTEST, __FILE_LINE__, format, ##arg)

#define logDebugAction(format, arg...) \
    logPrintAction(logDEBUG, format, ##arg); \
    logPrint(logDEBUG, __FILE_LINE__, format, ##arg)

#define logExtDbgAction(format, arg...) \
    logPrintAction(logEXTDBG, format, ##arg); \
    logPrint(logEXTDBG, __FILE_LINE__, format, ##arg)

#ifdef __cplusplus
};
#endif
  
#endif /*!logLog_H*/
