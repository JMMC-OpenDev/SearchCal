/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errGetLocalStackSize function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errGetLocalStackSize_L.c,v 1.6 2006-01-10 14:40:39 mella Exp $"; 


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
 * Returns number of errors in the stack.
 * 
 * \param error Error structure.
 *
 * \return number of errors in the stack.
 */
mcsINT8 errGetLocalStackSize (errERROR_STACK *error)
{
    logTrace("errGetLocalStackSize()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Returns the number of element in stack*/
    return (error->stackSize);
}

/*___oOo___*/

