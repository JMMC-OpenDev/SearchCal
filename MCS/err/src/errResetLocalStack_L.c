/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errResetLocalStack_L.c,v 1.3 2005-01-24 14:49:18 gzins Exp $"; 
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
mcsCOMPL_STAT errResetLocalStack(errERROR *error)
{
    mcsINT32 i;

    logExtDbg("errResetLocalStack()");

    /* Initialize the error structure */
    memset((char *)error, '\0', sizeof(errERROR));
    for ( i = 0; i < error->stackSize; i++)
    {
        memset((char *)error->stack[i].timeStamp, '\0',
                sizeof(error->stack[i].timeStamp));
        error->stack[i].sequenceNumber = -1;
        memset((char *)error->stack[i].procName, '\0',
                sizeof(error->stack[i].procName));
        memset((char *)error->stack[i].location, '\0',
                sizeof(error->stack[i].location));
        memset((char *)error->stack[i].moduleId, '\0',
                sizeof(error->stack[i].moduleId));
        error->stack[i].severity = ' ';
        memset((char *)error->stack[i].runTimePar, '\0',
                sizeof(error->stack[i].runTimePar));
    }
    error->stackSize = 0;
    error->stackOverflow = mcsFALSE;
    error->stackEmpty = mcsTRUE;

    error->thisPtr = error;

    return mcsSUCCESS;
}

/*___oOo___*/

