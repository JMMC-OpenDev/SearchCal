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
            mcsSTRING2048 log;
            mcsSTRING512 logBuf;

            memset(logBuf, '\0', sizeof(logBuf));

            snprintf(logBuf, sizeof(logBuf) - 1, "%d %c %s",
                    error->stack[i].sequenceNumber,
                    error->stack[i].severity,
                    error->stack[i].runTimePar);

            snprintf(log, sizeof(log) - 1, "ERROR: %s%s", tab, logBuf);
            
            /* TODO: give error stack timestamp to logPrint */
            logPrint(error->stack[i].moduleId, logERROR, error->stack[i].location, log);
            
            /* Add tab to show error message hierarchy */
            strcat(tab, " ");
        }
    }

    /* Print-out error stack : TODO: remove when logPrint is ok */
    errDisplayLocalStack(error);

    /* Re-initialise error stack */
    errResetLocalStack(error);

    return mcsSUCCESS;
}

/*___oOo___*/

