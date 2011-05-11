/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of errDisplayLocalStack function.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errDisplayLocalStack_L.c,v 1.9 2006-01-10 14:40:39 mella Exp $"; 


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
 * Displays the error stack.
 * 
 * \param  error Error structure to be displayed.
 */
mcsCOMPL_STAT errDisplayLocalStack(errERROR_STACK *error)
{
    mcsINT32 i;
    mcsSTRING32 tab;

    logTrace("errDisplayLocalStack()");
     
    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* For each error message */
    memset(tab , '\0', sizeof(tab)); 
    for ( i = 0; i < error->stackSize; i++)
    {
        /* Display error message */
        fprintf(stderr, "%s - %s %s %s %s %d %c %s\n",
                error->stack[i].timeStamp,
                error->stack[i].moduleId,
                error->stack[i].procName,
                tab,
                error->stack[i].location,
                error->stack[i].errorId,
                error->stack[i].severity,
                error->stack[i].runTimePar);

        /* Add tab to show error message hierarchy */
        strcat(tab, " ");
    }

    return mcsSUCCESS;
}

/*___oOo___*/

