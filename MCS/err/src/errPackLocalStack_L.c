/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errPackLocalStack_L.c,v 1.2 2004-08-23 13:37:03 gzins Exp $"; 
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
 * Re-initialize the error structure to start a new error stack.
 * 
 * \param  error Error structure to be reset.
 */
mcsCOMPL_STAT errPackLocalStack(errERROR *error, char *buffer,
                                mcsUINT32 bufSize)
{
    mcsINT32     i;
    mcsUINT32    bufLen;

    logExtDbg("errPackLocalStack()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    memset(buffer, '\0', bufSize);
    bufLen = 0;
    
    /* For each error message */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* Format error message */
        char log[errMSG_MAX_LEN];
        char logBuf[512];

        sprintf(logBuf,"%s %d %c %s",
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Check length */
        if (strlen(logBuf)  > errMSG_MAX_LEN)
        {
            logBuf[errMSG_MAX_LEN] = '\0';
        }

        memset(log, '\0', sizeof(log));
        sprintf(log,"%s - %s %s %.200s\n",
                error->stack[i].timeStamp, error->stack[i].moduleId,
                error->stack[i].procName, logBuf);

        /* Store message into buffer */
        if ((bufSize - bufLen) > strlen(log))
        {
            strncpy(&buffer[bufLen], log, strlen(log));
            bufLen += strlen(log);
        }
        else
        {
            logWarning("errStoreExtrBuffer() - buffer too small "
                       "(current size: %d  requested size: %d)",
                       bufSize, bufLen + strlen(log));
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*___oOo___*/

