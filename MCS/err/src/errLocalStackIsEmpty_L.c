/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.2  2005/01/24 14:49:18  gzins
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: errLocalStackIsEmpty_L.c,v 1.3 2005-01-27 14:10:50 gzins Exp $"; 
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
mcsLOGICAL errLocalStackIsEmpty (errERROR_STACK          *error)
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

