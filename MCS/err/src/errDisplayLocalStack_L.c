/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.8  2005/06/01 13:23:49  gzins
* Changed logExtDbg to logTrace
*
* Revision 1.7  2005/02/15 08:09:35  gzins
* Added file description
*
* Revision 1.6  2005/01/27 14:10:07  gzins
* Changed errERROR to errERROR_STACK
*
* Revision 1.5  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     18-Nov-2004  printed out error stack on stderr
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errDisplayLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errDisplayLocalStack_L.c,v 1.9 2006-01-10 14:40:39 mella Exp $"; 


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
 * Displays the error stack.
 * 
 * \param  error Error structure to be displayed.
 */
mcsCOMPL_STAT errDisplayLocalStack(errERROR_STACK *error)
{
    mcsINT32 i;
    mcsSTRING32 tab;

    logTrace("errDisplayLocalStack()");
     
    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* For each error message */
    memset(tab , '\0', sizeof(tab)); 
    for ( i = 0; i < error->stackSize; i++)
    {
        /* Display error message */
        fprintf(stderr, "%s - %s %s %s %s %d %c %s\n",
                error->stack[i].timeStamp,
                error->stack[i].moduleId,
                error->stack[i].procName,
                tab,
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Add tab to show error message hierarchy */
        strcat(tab, " ");
    }

    return mcsSUCCESS;
}

/*___oOo___*/

