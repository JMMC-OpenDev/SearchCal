/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errPushInLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errPushInLocalStack_L.c,v 1.8 2006-01-10 14:40:39 mella Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>

/*
 * Common Software Headers
 */
#include "mcs.h"
#include "log.h"

/*
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/**
 * Put an new error in the local stack error.
 * 
 * \param error error structure to be reset.
 * \param timeStamp time stamp when error occured.
 * \param procName process name which has produced this error.
 * \param moduleId module identifier
 * \param location file name and line number from where the error has been added
 * \param errorId error number
 * \param severity error severity (F, S or W)
 * \param runTimePar error message
 */
mcsCOMPL_STAT errPushInLocalStack(errERROR_STACK *error,
                                  const char     *timeStamp,
                                  const char     *procName,
                                  const char     *moduleId,
                                  const char     *location,
                                  mcsINT32       errorId,
                                  mcsLOGICAL     isErrUser,
                                  char           severity,
                                  char           *runTimePar)
{
    mcsINT32 errNum;

    logTrace("errPutInStack()"); 

    /* Check parameter */
    if (error  ==  NULL)
    {
        logWarning("Parameter error is a NULL pointer, module %s, "
                   "err. number %i, location %s", moduleId, errorId, location);
        return(mcsFAILURE);
    }

    /* If stack is full */
    if (error->stackSize == errSTACK_SIZE)
    {
        logWarning( "error stack is full: force error logging");

        /* Log the error messages */
        errCloseLocalStack(error);
    }

    /* Update the stack status (number of errors and state) */
    error->stackSize++;
    error->stackEmpty = mcsFALSE;

    /* Add error to the stack */
    errNum = error->stackSize - 1;
    error->stack[errNum].sequenceNumber = error->stackSize;
    strncpy((char *)error->stack[errNum].timeStamp, timeStamp,
            (sizeof(error->stack[errNum].timeStamp)-1));
    strncpy((char *)error->stack[errNum].procName, mcsGetProcName(),
            (sizeof(mcsPROCNAME)-1));
    strncpy((char *)error->stack[errNum].moduleId, moduleId,
            (sizeof(mcsMODULEID)-1));
    strncpy((char *)error->stack[errNum].location, location,
            (sizeof(mcsFILE_LINE) -1));
    strcpy((char *) error->stack[errNum].moduleId, moduleId);
    error->stack[errNum].errorId = errorId;
    error->stack[errNum].isErrUser = isErrUser;
    error->stack[errNum].severity = severity;
    strcpy((char *)error->stack[errNum].runTimePar, runTimePar);

    /* Display newly error added (for debug purpose only) */
    logDebug("%s - %s %s %s %d %c %s\n",
            error->stack[errNum].timeStamp,
            error->stack[errNum].moduleId,
            error->stack[errNum].procName,
            error->stack[errNum].location,
            error->stack[errNum].errorId,
            error->stack[errNum].severity,
            error->stack[errNum].runTimePar);

    return mcsSUCCESS;
}

/*___oOo___*/

