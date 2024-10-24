/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errPushInLocalStack function.
 */


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
 * \param timeStamp time stamp when error occurred.
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
                                  const char     *runTimePar)
{
    mcsINT32 errNum;

    /* Check parameter */
    if (error  ==  NULL)
    {
        logWarning("Parameter error is a NULL pointer, module %s, "
                   "err. number %i, location %s", moduleId, errorId, location);
        return (mcsFAILURE);
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
    strncpy((char *) error->stack[errNum].timeStamp, timeStamp, mcsLEN32 - 1);
    strncpy((char *) error->stack[errNum].procName, procName, mcsPROCNAME_LEN - 1);
    strncpy((char *) error->stack[errNum].moduleId, moduleId, mcsMODULEID_LEN - 1);
    strncpy((char *) error->stack[errNum].location, location, mcsFILE_LINE_LEN - 1);
    strncpy((char *) error->stack[errNum].moduleId, moduleId, mcsMODULEID_LEN - 1);
    
    error->stack[errNum].errorId = errorId;
    error->stack[errNum].isErrUser = isErrUser;
    error->stack[errNum].severity = severity;
    strncpy((char *) error->stack[errNum].runTimePar, runTimePar, mcsLEN256 - 1);

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

