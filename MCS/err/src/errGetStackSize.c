/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errGetStackSize function.
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
 * Returns number of errors in the global error stack.
 *
 * This routines returns number of errors currently strored in the global
 * error stack. 
 * 
 * \return number of errors in the global error stack.
 */
mcsINT8 errGetStackSize()
{
    logTrace("errGetStackSize()");

    return (errGetLocalStackSize(errGetThreadStack()));
}

/*___oOo___*/

