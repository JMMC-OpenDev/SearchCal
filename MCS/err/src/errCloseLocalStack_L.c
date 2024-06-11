/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errCloseLocalStack function.
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
 * Logs errors and resets the local error structure.
 *
 * \param  error Error structure containing previous error context.
 */
mcsCOMPL_STAT errCloseLocalStack(errERROR_STACK *error)
{
    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* If error stack is initialised */
    if (error->stackInit == mcsTRUE)
    {
        mcsINT32     i;
        mcsSTRING128 tab;

        memset(tab, '\0', sizeof (tab));

        /* For each error message */
        for ( i = 0; i < error->stackSize; i++)
        {
            /* Format the  message */
            logPrint(error->stack[i].moduleId, logERROR, error->stack[i].timeStamp,
                     error->stack[i].location, "%sERROR: %d %c %s", tab,
                     error->stack[i].sequenceNumber,
                     error->stack[i].severity,
                     error->stack[i].runTimePar);

            /* Add tab to show error message hierarchy */
            strcat(tab, " ");
        }
    }

    /* Re-initialise error stack */
    errResetLocalStack(error);

    return mcsSUCCESS;
}

/*___oOo___*/

