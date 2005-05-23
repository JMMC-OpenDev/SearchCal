#ifndef miscNetwork_H
#define miscNetwork_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscNetwork.h,v 1.4 2005-05-23 11:57:40 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  03-Aug-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Definition of miscNetwork functions.
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
 
mcsCOMPL_STAT miscGetHostName(char *hostName, const mcsUINT32 length);


#ifdef __cplusplus
}
#endif

#endif /*!miscNetwork_H*/

/*___oOo___*/
