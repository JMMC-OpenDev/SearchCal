/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errCloseStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Logs errors and resets the global error structure.
 *
 * It logs all errors which have been placed in the global error stack and
 * reset it. This has to be done when the last error of the sequence cannot be
 * recovered.
 * \return SUCCESS or FAILURE if an error occured.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errCloseStack()
{
    logExtDbg("errCloseStack()");

    return (errCloseLocalStack(&errGlobalStack));
}

/*___oOo___*/

