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
 * Definition of errAddInStack function.
 */
static char *rcsId="@(#) $Id: errAddInStack.c,v 1.3 2005-01-24 14:45:09 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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
 * Add a new log to the current error stack
 *
 * It logs all errors which have been placed in the global error stack and
 * reset it. This has to be done when the last error of the sequence cannot be
 * recovered.
 * \return mcsSUCCESS or mcsFAILURE if an error occured.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errAddInStack(const mcsMODULEID moduleId,
                            const char        *fileLine,
                            mcsINT32          errorId, ...)
{
    va_list       argPtr;
    mcsCOMPL_STAT status;

    logExtDbg("errAddInStack(%s, %d)", moduleId, errorId);
    /* Call the error message */
    va_start(argPtr, errorId);
    status = errAddInLocalStack_v(&errGlobalStack, moduleId, 
                                  fileLine, errorId, argPtr);
    va_end(argPtr);

    return (status);
}

/*___oOo___*/

