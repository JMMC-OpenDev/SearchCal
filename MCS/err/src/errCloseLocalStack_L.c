/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.7  2005/06/01 13:23:49  gzins
* Changed logExtDbg to logTrace
*
* Revision 1.6  2005/02/15 08:09:35  gzins
* Added file description
*
* Revision 1.5  2005/01/27 14:09:48  gzins
* Changed errERROR to errERROR_STACK
*
* Revision 1.4  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     06-Dec-2004  printed-out error stack when closing it
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errCloseLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errCloseLocalStack_L.c,v 1.8 2006-01-10 14:40:39 mella Exp $"; 


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

    return mcsSUCCESS;
}

/*___oOo___*/

