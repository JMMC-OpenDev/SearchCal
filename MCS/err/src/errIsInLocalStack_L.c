/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.4  2005/01/31 15:51:18  mella
* Correct typo error
*
* Revision 1.3  2005/01/27 14:10:37  gzins
* Changed errERROR to errERROR_STACK
*
* Revision 1.2  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errIsInLocalStack function.
 */

static char *rcsId="@(#) $Id: errIsInLocalStack_L.c,v 1.5 2005-02-15 08:09:35 gzins Exp $"; 
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
 * Checks if the error stack contains a given error
 * 
 * \param error Error structure containing current error context.
 * \param moduleId  Module identifier
 * \param errorId Error number
 *
 * \return mcsTRUE if given error is in stack, or mcsFALSE otherwise.
 */
mcsLOGICAL errIsInLocalStack (errERROR_STACK    *error,
                              const mcsMODULEID moduleId,
                              mcsINT32          errorId)
{
    mcsINT32 i;

    logExtDbg("errIsInLocalStack()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
        return mcsFALSE;
    } 

    /* For each error in stack */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* If module names match */
        if (strcmp(error->stack[i].moduleId, moduleId) == 0)
        {
            /* If error Ids match */
            if (error->stack[i].errorId == errorId)
            {
                /* Return TRUE */
                return mcsTRUE;
            }
        }
    }

    /* Else return FALSE */
    return mcsFALSE;
}
/*___oOo___*/

