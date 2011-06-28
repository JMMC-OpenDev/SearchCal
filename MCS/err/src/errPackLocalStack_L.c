/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errPackLocalStack function.
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
 * Re-initialize the error structure to start a new error stack.
 * 
 * \param  error Error structure to be reset.
 */
mcsCOMPL_STAT errPackLocalStack(errERROR_STACK *error, char *buffer,
                                mcsUINT32 bufSize)
{
    mcsINT32     i;
    mcsUINT32    bufLen;

    logTrace("errPackLocalStack()");
    
    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
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

        sprintf(logBuf,"%s %d %d %c %s",
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].isErrUser,
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
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/

