/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errIsInStack function.
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
 * Checks is the global error stack contains a given error
 *
 * This routines checks if a given error is part of the global error stack :
 * the error is univocally addressed by the moduleId and the errorId. 
 * 
 * \param moduleId Module identifier
 * \param errorId  Error number
 *
 * \return mcsTRUE if the error is in the stack otherwise mcsFALSE.
 */
mcsLOGICAL errIsInStack(const mcsMODULEID moduleId,
                           mcsINT32          errorId)
{
    logTrace("errIsInStack()");

    return (errIsInLocalStack(errGetThreadStack(), moduleId, errorId));
}

/*___oOo___*/

