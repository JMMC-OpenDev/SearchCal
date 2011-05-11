/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errPackStack function.
 */
static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errPackStack.c,v 1.4 2006-01-10 14:40:39 mella Exp $"; 


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
    logTrace("errPackStack()");

    return (errPackLocalStack(&errGlobalStack, buffer, bufLen));
}

/*___oOo___*/

