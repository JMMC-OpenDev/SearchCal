#ifndef miscNetwork_H
#define miscNetwork_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscNetwork.h,v 1.2 2005-01-19 10:29:42 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Aug-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * This header contains ONLY the miscNetwork functions declarations.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/* 
 * MCS Headers
 */
#include "mcs.h"


/*
 * Pubic functions declaration
 */
 
mcsCOMPL_STAT miscGetHostName(char *hostName, mcsUINT32 length);


#ifdef __cplusplus
}
#endif

#endif /*!miscNetwork_H*/

/*___oOo___*/
