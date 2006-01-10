/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/06/01 13:23:49  gzins
* Changed logExtDbg to logTrace
*
* Revision 1.5  2005/02/15 08:09:35  gzins
* Added file description
*
* Revision 1.4  2005/01/27 14:12:44  gzins
* Changed errERROR to errERROR_STACK
*
* Revision 1.3  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errResetLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errResetLocalStack_L.c,v 1.7 2006-01-10 14:40:39 mella Exp $"; 


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
 *
 * \return mcsSUCCESS.
 */
mcsCOMPL_STAT errResetLocalStack(errERROR_STACK *error)
{
    mcsINT32 i;

    logTrace("errResetLocalStack()");

    /* Initialize the error structure */
    memset((char *)error, '\0', sizeof(errERROR_STACK));
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

