/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errGetStackSize function.
 */
static char *rcsId="@(#) $Id: errGetStackSize.c,v 1.2 2005-01-24 14:45:09 gzins Exp $"; 
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

