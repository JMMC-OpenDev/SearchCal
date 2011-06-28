/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errLocalStackIsEmpty function.
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
 * Checks is the error stack is empty 
 * 
 * \param error Error structure.
 *
 * \return mcsTRUE if the error stack is empty, otherwise mcsFALSE.
 */
mcsLOGICAL errLocalStackIsEmpty (errERROR_STACK          *error)
{
    logTrace("errLocalStackIsEmpty()");
    
    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
    {
        errResetLocalStack(error);
    } 

    /* Returns empty flag */
    return (error->stackEmpty);
}
/*___oOo___*/

