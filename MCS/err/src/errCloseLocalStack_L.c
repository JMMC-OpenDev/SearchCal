/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errCloseLocalStack function.
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

/*
 * Logs errors and resets the local error structure.
 * 
 * \param  error Error structure containing previous error context.
 */
mcsCOMPL_STAT errCloseLocalStack(errERROR_STACK *error)
{
    mcsINT32     i;
    mcsSTRING128 tab;

    logTrace("errCloseLocalStack()");
 
    if (error == NULL)
    {
        return mcsFAILURE;
    }
    
    /* If error stack is initialised */
    if (error->stackInit == mcsTRUE)
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

            /* TODO : CLEANUP : why log so many times errors (push, close, add ...) in stederr / out / socket ...*/
            /* WHY NOT USE log module instead ???? */
            
            /* Send message to log system */
            logData(error->stack[i].moduleId,
                     logERROR,
                     error->stack[i].timeStamp, 
                     error->stack[i].location, log);

            /* Add tab to show error message hierarchy */
            strcat(tab, " ");
        }
    }

    /* Print-out error stack */
    errDisplayLocalStack(error);

    /* Re-initialise error stack */
    errResetLocalStack(error);

    return mcsSUCCESS;
}

/*___oOo___*/

