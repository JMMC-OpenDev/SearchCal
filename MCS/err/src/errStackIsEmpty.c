/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errStackIsEmpty function.
 */

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
    return (errLocalStackIsEmpty(errGetThreadStack()));
}

/*___oOo___*/

