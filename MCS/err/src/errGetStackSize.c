/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errGetStackSize.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Returns number of errors in the global error stack.
 *
 * This routines returns number of errors currently strored in the global
 * error stack. 
 * 
 * \return number of errors in the global error stack.
 */
mcsINT8 errGetStackSize()
{
    logExtDbg("errGetStackSize()");

    return (errGetLocalStackSize(&errGlobalStack));
}

/*___oOo___*/

