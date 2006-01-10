/*******************************************************************************
* JMMC project
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.3  2005/06/01 13:23:49  gzins
* Changed logExtDbg to logTrace
*
* Revision 1.2  2005/01/24 14:45:09  gzins
* Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
* Used CVS log as modification history
*
* gzins     17-Jun-2004  completed implementation
* berezne   02-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Definition of errCloseStack function.
 */
static char *rcsId __attribute__ ((unused)) = "@(#) $Id: errCloseStack.c,v 1.4 2006-01-10 14:40:39 mella Exp $"; 


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
 * Logs errors and resets the global error structure.
 *
 * It logs all errors which have been placed in the global error stack and
 * reset it. This has to be done when the last error of the sequence cannot be
 * recovered.
 * \return mcsSUCCESS or mcsFAILURE if an error occured.
 *
 * \sa errAdd, errResetStack
 */
mcsCOMPL_STAT errCloseStack()
{
    logTrace("errCloseStack()");

    return (errCloseLocalStack(&errGlobalStack));
}

/*___oOo___*/

