#ifndef alxPrivate_H
#define alxPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: alxPrivate.h,v 1.1 2005-01-20 10:48:51 sccmgr Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* sccmgr    20-Jan-2005  Created
*
*
*******************************************************************************/

/**
 * \file
 * alx private header file.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Module name
 */
#define MODULE_ID "alx"

 

#ifdef __cplusplus
}
#endif


#endif /*!alxPrivate_H*/

/*___oOo___*/
