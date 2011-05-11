/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errResetStack function.
 */
static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errResetStack.c,v 1.5 2006-01-10 14:40:39 mella Exp $"; 


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
 * It reset the error stack; i.e. removed all errors from the stack. This has to
 * be done when the error has been handled by application or it is simply
 * ignored. 
 *
 * \return mcsSUCCESS or mcsFAILURE if an error occured.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errResetStack()
{
    logTrace("errResetStack()");

    return (errResetLocalStack(&errGlobalStack));
}

/*___oOo___*/

