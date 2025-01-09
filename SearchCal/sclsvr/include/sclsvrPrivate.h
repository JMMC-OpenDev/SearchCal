#ifndef sclsvrPrivate_H
#define sclsvrPrivate_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Brief description of the header file, which ends at this dot.
 */
#include "mcs.h"
#include "thrd.h"
#include "sdb.h"
#include "msg.h"
#include "sclsvrSERVER.h"

#include "math.h"

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
 */
#ifdef __cplusplus
extern "C"
{
#endif


/*
 * Module name
 */
#define MODULE_ID "sclsvr"

// Tag used to stored request into file
#define sclsvrREQUEST_TAG "# REQUEST = "

// Tag used to stored format into file
#define sclsvrFORMAT_TAG  "# FORMAT = "

/* Discard time counter */
#define FAIL_TIMLOG_CANCEL(status, cmdName) {   \
    FAIL_DO(status,                             \
            timlogCancel(cmdName))              \
}

#define TIMLOG_CANCEL(cmdName) {                \
    timlogCancel(cmdName);                      \
    return mcsFAILURE;                          \
}

/**
 * Parameters for the monitoring task
 */
typedef struct
{
    sclsvrSERVER* server; /** pointer on a sclsvrSERVER instance. */
    msgMESSAGE*  message; /** pointer on a msgMESSAGE instance. */
    sdbENTRY*     status; /** pointer on a sdbENTRY instance. */

} sclsvrMONITOR_TASK_PARAMS;

#ifdef __cplusplus
}
#endif

// Monitoring task entry function
thrdFCT_RET sclsvrMonitorTask(thrdFCT_ARG param);


#endif /*!sclsvrPrivate_H*/

/*___oOo___*/
