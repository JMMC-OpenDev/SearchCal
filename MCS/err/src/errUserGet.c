/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errUserGet function.
 */


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
 * Get the last end-user oriented error message stored in error stack.
 *
 * It looks for the last end-user oriented error message placed in the stack;
 * i.e. added using errUserAdd(). If not found in stack, it returns the last
 * added error. If the stack is empty, NULL is returned.
 *
 * \return last user error message or NULL if stack is empty.
 *
 * \sa errUserAdd
 */
char *errUserGet(void)
{
    logTrace("errUserGet()");

    /* Return user message stored in the global stack */
    return (errUserGetInLocalStack(errGetThreadStack()));
}



/*___oOo___*/
