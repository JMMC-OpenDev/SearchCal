#ifndef logLOG_PRIVATE_H
#define logLOG_PRIVATE_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Private log module header file, holding the MODULE_NAME definition, logRULE
 * structure definition, contants and local-to-module functions declarations.
 */

/* The following piece of code alternates the linkage type to C for all
functions declared within the braces, which is necessary to use the
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

/*
 * MCS Headers 
 */
#include "mcs.h"


/*
 * Local Headers 
 */
#include "log.h"


/*
 * Constants
 */
#define MODULE_ID    "log"

/**
 * logManager default listened network port number.
 */
#define logMANAGER_DEFAULT_PORT_NUMBER 8791

/*
 * Define logging definition structure 
 */
typedef struct {
        mcsSTRING256 logManagerHostName;
        mcsUINT32   logManagerPortNumber;
        mcsLOGICAL  log;
        mcsLOGICAL  verbose;
        logLEVEL    logLevel;
        logLEVEL    verboseLevel;
        logLEVEL    actionLevel;
        mcsLOGICAL  printDate;
        mcsLOGICAL  printFileLine;
        mcsLOGICAL  printThreadName;
} logRULE;

/*
 * Local Functions
 */
mcsCOMPL_STAT logGetHostName     (      char *, mcsUINT32);
void          logPrintErrMessage (const char *, ...);

#ifdef __cplusplus
};
#endif
  
#endif /*!logLOG_PRIVATE_H*/

/*___oOo___*/
