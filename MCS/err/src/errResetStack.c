/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errResetStack function.
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
 * Logs errors and resets the global error structure.
 *
 * It reset the error stack; i.e. removed all errors from the stack. This has to
 * be done when the error has been handled by application or it is simply
 * ignored.
 *
 * \return mcsSUCCESS or mcsFAILURE if an error occurred.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errResetStack()
{
    return (errResetLocalStack(errGetThreadStack()));
}

/*___oOo___*/

