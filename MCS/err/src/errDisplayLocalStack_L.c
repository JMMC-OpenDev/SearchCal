/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errDisplayLocalStack_L.c,v 1.2 2004-07-22 10:08:30 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
mcsCOMPL_STAT errDisplayLocalStack(errERROR *error)
{
    mcsINT32 i;
    mcsSTRING32 tab;

    logExtDbg("errDisplayLocalStack()");
     
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
        printf("%s - %s %s %s %s %d %c %s\n",
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

    return SUCCESS;
}

/*___oOo___*/

