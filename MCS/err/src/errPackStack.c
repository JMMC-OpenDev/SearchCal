/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errPackStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Packs global error structure to a buffer.
 *
 * This routines packs the error information into a buffer.
 * 
 * \param buffer Pointer to buffer where the error structure has to be packed
 * \param bufLen The size of buffer
 *
 * \return SUCESSS, or FAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errPackStack(char       *buffer,
                           mcsUINT32  bufLen)
{
    logExtDbg("errPackStack()");

    return (errPackLocalStack(&errGlobalStack, buffer, bufLen));
}

/*___oOo___*/

