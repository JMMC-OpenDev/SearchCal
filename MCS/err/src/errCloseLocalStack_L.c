/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
* gzins     06-Dec-2004  printed-out error stack when closing it
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errCloseLocalStack_L.c,v 1.2 2004-12-06 14:13:13 gzins Exp $"; 
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

/*
 * Logs errors and resets the local error structure.
 * 
 * \param  error Error structure containing previous error context.
 */
mcsCOMPL_STAT errCloseLocalStack(errERROR *error)
{
    mcsINT32     i;
    mcsSTRING128 tab;

    logExtDbg("errCloseLocalStack()");
 
    /* If error stack is initialised */
    if (error->thisPtr == error)
    {
        memset(tab, '\0', sizeof(tab));

        /* For each error message */
        for ( i = 0; i < error->stackSize; i++)
        {
            /* Format the  message */
            char log[errMSG_MAX_LEN];
            char logBuf[512];

            memset(logBuf, '\0', sizeof(logBuf));

            sprintf(logBuf,"%d %c %s",
                    error->stack[i].sequenceNumber,
                    error->stack[i].severity,
                    error->stack[i].runTimePar);

            /* Check length */
            if ((strlen(logBuf) + strlen(tab)) > errMSG_MAX_LEN)
            {
                logBuf[errMSG_MAX_LEN - strlen(tab)] = '\0';
            }

            sprintf(log,"%s%s", tab, logBuf);

            /* Send message to log system */
            logData (error->stack[i].moduleId,
                     logERROR,
                     error->stack[i].timeStamp, 
                     error->stack[i].location, log);

            /* Add tab to show error message hierarchy */
            strcat(tab, " ");
        }
    }

    /* Print-out error stack */
    errDisplayStack();

    /* Re-initialise error stack */
    errResetLocalStack(error);

    return SUCCESS;
}

/*___oOo___*/

