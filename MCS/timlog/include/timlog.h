#ifndef timlog_H
#define timlog_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: timlog.h,v 1.1 2004-12-17 10:06:44 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     17-Dec-2004  Created
*
*******************************************************************************/

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
void timlogStart(const logLEVEL level, const char* actionName);
void timlogStop(const char* actionName);
void timlogClear();

#ifdef __cplusplus
}
#endif

#endif /*!timlog_H*/

/*___oOo___*/
