/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errUnpackStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Extracts the error structure from buffer
 *
 * This routines extracts the error information from buffer, and stored the
 * extracted errors in the global stack structure.
 * 
 * \param buffer Pointer to buffer where the error structure has been packed
 * \param bufLen The size of buffer
 *
 * \return SUCESSS, or FAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errUnpackStack(char       *buffer,
                           mcsUINT32  bufLen)
{
    logExtDbg("errPackStack()");

    return (errUnpackLocalStack(&errGlobalStack, buffer, bufLen));
}

/*___oOo___*/

