/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.2  2005/01/24 14:45:09  gzins
* Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errUnpackStack function.
 */
static char *rcsId="@(#) $Id: errUnpackStack.c,v 1.3 2005-02-09 14:26:03 lafrasse Exp $"; 
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
 * \return mcsSUCCESS, or mcsFAILURE if the buffer size is too small.
 */
mcsCOMPL_STAT errUnpackStack(const char *buffer,
                             mcsUINT32  bufLen)
{
    logExtDbg("errPackStack()");

    return (errUnpackLocalStack(&errGlobalStack, buffer, bufLen));
}

/*___oOo___*/

