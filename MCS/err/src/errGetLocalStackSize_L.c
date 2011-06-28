/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errGetLocalStackSize function.
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
 * Returns number of errors in the stack.
 * 
 * \param error Error structure.
 *
 * \return number of errors in the stack.
 */
mcsINT8 errGetLocalStackSize (errERROR_STACK *error)
{
    logTrace("errGetLocalStackSize()");
    
    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
    {
        errResetLocalStack(error);
    } 

    /* Returns the number of element in stack*/
    return (error->stackSize);
}

/*___oOo___*/

