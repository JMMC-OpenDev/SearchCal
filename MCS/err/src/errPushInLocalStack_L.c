/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errPushInLocalStack_L.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
mcsCOMPL_STAT errPushInLocalStack(errERROR   *error,
                                  const char *timeStamp,
                                  const char *procName,
                                  const char *moduleId,
                                  const char *location,
                                  mcsINT32   errorId,
                                  char       severity,
                                  char       *runTimePar)
{
    mcsINT32 errNum;

    logExtDbg("errPutInStack()"); 

    /* Check parameter */
    if (error  ==  NULL)
    {
        logWarning("Parameter error is a NULL pointer, module %s, "
                   "err. number %i, location %s", moduleId, errorId, location);
        return(FAILURE);
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
    error->stack[errNum].severity = severity;
    strcpy((char *)error->stack[errNum].runTimePar, runTimePar);
 
    return SUCCESS;
}

/*___oOo___*/

