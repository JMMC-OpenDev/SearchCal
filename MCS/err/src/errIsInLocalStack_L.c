/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errIsInLocalStack_L.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Checks is the error stack contains a given error
 * 
 * \param error Error structure containing current error context.
 * \param moduleId  Module identifier
 * \param errorId Error number
 */
mcsLOGICAL errIsInLocalStack (errERROR          *error,
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

