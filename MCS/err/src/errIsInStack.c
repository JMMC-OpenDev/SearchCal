/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errIsInStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Checks is the global error stack contains a given error
 *
 * This routines checks if a given error is part of the global error stack :
 * the error is univocally addressed by the moduleId and the errorId. 
 * 
 * \param moduleId Module identifier
 * \param errorId  Error number
 *
 * \return mcsTRUE if the error is in the stack otherwise mcsFALSE.
 */
mcsLOGICAL errIsInStack(const mcsMODULEID moduleId,
                           mcsINT32          errorId)
{
    logExtDbg("errIsInStack()");

    return (errIsInLocalStack(&errGlobalStack, moduleId, errorId));
}

/*___oOo___*/

