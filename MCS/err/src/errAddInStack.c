/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errAddInStack.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * \return SUCCESS or FAILURE if an error occured.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errAddInStack(const mcsMODULEID moduleId,
                            const char        *fileLine,
                            mcsINT32          errorId, ...)
{
    va_list       argPtr;
    mcsCOMPL_STAT status;

    logExtDbg("errAddInStack()");
    /* Call the error message */
    va_start(argPtr, errorId);
    status = errAddInLocalStack_v(&errGlobalStack, moduleId, 
                                  fileLine, errorId, argPtr);
    va_end(argPtr);

    return (status);
}

/*___oOo___*/

