#ifndef misco_H
#define misco_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: misco.h,v 1.2 2005-02-11 09:37:23 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/02/10 13:34:06  sccmgr
 * Fix directory structure and add additional files
 *
 ******************************************************************************/

/**
 * \file
 * Main header file, grouping all the public headers of this module library.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Local headers
 */
#include "miscoDYN_BUF.h"
 

#ifdef __cplusplus
}
#endif


#endif /*!misco_H*/

/*___oOo___*/
