#ifndef miscPrivate_H
#define miscPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscPrivate.h,v 1.1 2004-06-24 13:47:18 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     24-Jun-2004  Created
*
*
*******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Module name */ 
#define MODULE_ID "misc"

#ifdef __cplusplus
}
#endif

#endif /*!miscPrivate_H*/

/*___oOo___*/
