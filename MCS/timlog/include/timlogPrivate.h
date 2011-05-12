#ifndef timlogPrivate_H
#define timlogPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Private declaration for timer log module.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Module name
 */
#define MODULE_ID "timlog"

#include <sys/time.h>
 
/* Structure to hold one actionName<->startTime relation. */ 
typedef struct 
{
    mcsMODULEID    moduleName;
    mcsSTRING128   fileLine;
    mcsSTRING64    actionName;
    logLEVEL       level;
    struct timeval startTime;
} timlogENTRY;


#ifdef __cplusplus
}
#endif


#endif /*!timlogPrivate_H*/

/*___oOo___*/
