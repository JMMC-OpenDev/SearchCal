/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* berezne   02-Jun-2004  created
* gzins     17-Jun-2004  completed implementation
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errLocalStackIsEmpty_L.c,v 1.1 2004-06-23 13:04:15 gzins Exp $"; 
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
 * Checks is the error stack is empty 
 * 
 * \param error Error structure.
 *
 * \return mcsTRUE if the error stack is empty, otherwise mcsFALSE.
 */
mcsLOGICAL errLocalStackIsEmpty (errERROR          *error)
{
    logExtDbg("errLocalStackIsEmpty()");

    /* If error stack is not initialised, do it */
    if (error->thisPtr != error)
    {
        errResetLocalStack(error);
    } 

    /* Returns empty flag */
    return (error->stackEmpty);
}
/*___oOo___*/

