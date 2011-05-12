#ifndef timlog_H
#define timlog_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Declaration of timer log functions.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"

/*
 * Public functions declaration
 */
void timlogStart(const mcsMODULEID moduleName,const logLEVEL level, 
                 const char *fileLine, const char* actionName);
void timlogStop(const char* actionName);
void timlogClear();

/*
 * Convenience macros (see log)
 */
#define timlogInfoStart(action) \
    timlogStart(MODULE_ID, logINFO, __FILE_LINE__, action)

#define timlogTestStart(action) \
    timlogStart(MODULE_ID, logTEST, __FILE_LINE__, action)

#define timlogDebugStart(action) \
    timlogStart(MODULE_ID, logDEBUG, __FILE_LINE__, action)

#define timlogTraceStart(action) \
    timlogStart(MODULE_ID, logTRACE, __FILE_LINE__, action)

#ifdef __cplusplus
}
#endif

#endif /*!timlog_H*/

/*___oOo___*/
