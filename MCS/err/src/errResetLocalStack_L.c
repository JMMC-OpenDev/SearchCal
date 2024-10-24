/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errResetLocalStack function.
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
 * Re-initialize the error structure to start a new error stack.
 *
 * \param  error Error structure to be reset.
 *
 * \return mcsSUCCESS.
 */
mcsCOMPL_STAT errResetLocalStack(errERROR_STACK *error)
{
    mcsINT32 i;

    if (error == NULL)
    {
        return mcsFAILURE;
    }

    /* Initialize the error structure */
    memset((char *) error, '\0', sizeof (errERROR_STACK));
    
    for ( i = 0; i < error->stackSize; i++)
    {
        memset((char *) error->stack[i].timeStamp, '\0', mcsLEN32);
        error->stack[i].sequenceNumber = -1;
        memset((char *) error->stack[i].procName, '\0', mcsPROCNAME_LEN);
        memset((char *) error->stack[i].location, '\0', mcsFILE_LINE_LEN);
        memset((char *) error->stack[i].moduleId, '\0', mcsMODULEID_LEN);
        error->stack[i].severity = ' ';
        memset((char *) error->stack[i].runTimePar, '\0', mcsLEN256);
    }
    
    error->stackSize = 0;
    error->stackOverflow = mcsFALSE;
    error->stackEmpty = mcsTRUE;
    error->stackInit = mcsTRUE;

    return mcsSUCCESS;
}

/*___oOo___*/

