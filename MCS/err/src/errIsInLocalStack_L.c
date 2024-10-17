/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errIsInLocalStack function.
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
 * Checks if the error stack contains the given error
 *
 * \param error Error structure containing current error context.
 * \param moduleId  Module identifier
 * \param errorId Error number
 *
 * \return mcsTRUE if given error is in stack, or mcsFALSE otherwise.
 */
mcsLOGICAL errIsInLocalStack (errERROR_STACK    *error,
                              const mcsMODULEID moduleId,
                              mcsINT32          errorId)
{
    mcsINT32 i;

    if (error == NULL)
    {
        return mcsFALSE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
    {
        errResetLocalStack(error);
        return mcsFALSE;
    }

    /* For each error in stack */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* If module names match */
        if (strcmp(error->stack[i].moduleId, moduleId) == 0)
        {
            /* If error Ids match */
            if (error->stack[i].errorId == errorId)
            {
                /* Return TRUE */
                return mcsTRUE;
            }
        }
    }

    /* Else return FALSE */
    return mcsFALSE;
}

/**
 * Checks if the error stack contains the given error and returns its runtime message
 *
 * \param error Error structure containing current error context.
 * \param moduleId  Module identifier
 * \param errorId Error number
 * \param message returned error message (pointer to internal C string)
 *
 * \return mcsTRUE if given error is in stack, or mcsFALSE otherwise.
 */
mcsLOGICAL errGetInLocalStack (errERROR_STACK    *error,
                               const mcsMODULEID moduleId,
                               mcsINT32          errorId,
                               mcsSTRING256*     message)
{
    mcsINT32 i;

    if (error == NULL)
    {
        return mcsFALSE;
    }
    if (message == NULL)
    {
        return mcsFALSE;
    }

    /* If error stack is not initialised, do it */
    if (error->stackInit == mcsFALSE)
    {
        errResetLocalStack(error);
        return mcsFALSE;
    }

    /* For each error in stack */
    for ( i = 0; i < error->stackSize; i++)
    {
        /* If module names match */
        if (strcmp(error->stack[i].moduleId, moduleId) == 0)
        {
            /* If error Ids match */
            if (error->stack[i].errorId == errorId)
            {
                strcpy(*message, error->stack[i].runTimePar);

                /* Return TRUE */
                return mcsTRUE;
            }
        }
    }

    /* Else return FALSE */
    return mcsFALSE;
}

/*___oOo___*/

