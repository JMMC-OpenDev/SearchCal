/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errStackIsEmpty function.
 */
static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errStackIsEmpty.c,v 1.4 2006-01-10 14:40:39 mella Exp $"; 


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
 * Checks if the global error stack is empty 
 *
 * This routines checks if the global error stack is empty. 
 * 
 * \return mcsTRUE if the global error stack is empty, otherwise mcsFALSE.
 */
mcsLOGICAL errStackIsEmpty()
{
    logTrace("errStackIsEmpty()");

    /* Returns empty flag */
    return (errLocalStackIsEmpty(&errGlobalStack));
}

/*___oOo___*/

