/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errDisplayStack function.
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

    return (errDisplayLocalStack(errGetThreadStack()));
}

/*___oOo___*/

