/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errPackStack function.
 */
static char *rcsId="@(#) $Id: errPackStack.c,v 1.2 2005-01-24 14:45:09 gzins Exp $"; 
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
 * \return mcsSUCCESS, or mcsFAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errPackStack(char       *buffer,
                           mcsUINT32  bufLen)
{
    logExtDbg("errPackStack()");

    return (errPackLocalStack(&errGlobalStack, buffer, bufLen));
}

/*___oOo___*/

