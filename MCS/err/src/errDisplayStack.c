/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errDisplayStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Public function definition
 */
/**
 * Displays the error stack.
 *
 * It displays, on stdout, the list of errors currently stored in the global
 * error stack.
 *
 * \return SUCCESS.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errDisplayStack()
{
    logExtDbg("errDisplayStack()");

    return (errDisplayLocalStack(&errGlobalStack));
}

/*___oOo___*/

