/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.3  2005/01/27 14:10:18  gzins
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
 * Definition of errGetLocalStackSize function.
 */

static char *rcsId="@(#) $Id: errGetLocalStackSize_L.c,v 1.4 2005-02-15 08:09:35 gzins Exp $"; 
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

/*
 * Returns number of errors in the stack.
 * 
 * \param error Error structure.
 *
 * \return number of errors in the stack.
 */
mcsINT8 errGetLocalStackSize (errERROR_STACK *error)
{
    logExtDbg("errGetLocalStackSize()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Returns the number of element in stack*/
    return (error->stackSize);
}

/*___oOo___*/

