/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errStackIsEmpty.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Checks if the global error stack is empty 
 *
 * This routines checks if the global error stack is empty. 
 * 
 * \return mcsTRUE if the global error stack is empty, otherwise mcsFALSE.
 */
mcsLOGICAL errStackIsEmpty()
{
    logExtDbg("errStackIsEmpty()");

    /* Returns empty flag */
    return (errLocalStackIsEmpty(&errGlobalStack));
}

/*___oOo___*/

