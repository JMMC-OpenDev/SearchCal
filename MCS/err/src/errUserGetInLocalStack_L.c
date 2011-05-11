/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errUserGetInLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errUserGetInLocalStack_L.c,v 1.4 2006-01-10 14:40:39 mella Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>

/*
 * MCS Headers 
 */
#include "log.h"
#include "err.h"

/* 
 * Local Headers
 */
#include "err.h"
#include "errPrivate.h"

/*
 * Public functions definition
 */
/**
 * Get the user message stored in error stack.
 *
 * It lookup in the error stack the last added error message intended to the
 * end-user. If not found, it returns the last added error. If the stack is
 * empty, NULL is returned.

 * \return last user error message or NULL if stack is empty.
 *
 * \sa errUserGet
 */
char *errUserGetInLocalStack(errERROR_STACK   *error)
{
    logTrace("errUserGetMsgInLocalStack()");

    mcsINT32 userErrorIdx;
    mcsINT32 i;

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 
    
    /* If stack is empty, return NULL */
    if (error->stackEmpty == mcsTRUE)
    {
        return NULL;
    }
    
    /* Look for user error message */
    userErrorIdx = error->stackSize - 1;
    for ( i = 0; i < error->stackSize; i++)
    {
        if (error->stack[i].isErrUser == mcsTRUE)
        {
            userErrorIdx = i;
        }
    }

    /* Return user message stored in the stack */
    return (error->stack[userErrorIdx].runTimePar);
}



/*___oOo___*/
