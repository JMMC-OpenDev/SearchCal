#ifndef envPrivate_H
#define envPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envPrivate.h,v 1.1 2004-12-07 15:55:54 sccmgr Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* sccmgr    07-Dec-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Brief description of the header file, which ends at this dot.
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
#define MODULE_ID "env"

 

#ifdef __cplusplus
}
#endif


#endif /*!envPrivate_H*/

/*___oOo___*/
