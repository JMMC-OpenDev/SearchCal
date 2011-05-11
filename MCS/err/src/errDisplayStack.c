/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errDisplayStack function.
 */
static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errDisplayStack.c,v 1.4 2006-01-10 14:40:39 mella Exp $"; 


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

/*
 * Public function definition
 */
/**
 * Displays the error stack.
 *
 * It displays, on stdout, the list of errors currently stored in the global
 * error stack.
 *
 * \return mcsSUCCESS.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errDisplayStack()
{
    logTrace("errDisplayStack()");

    return (errDisplayLocalStack(&errGlobalStack));
}

/*___oOo___*/

