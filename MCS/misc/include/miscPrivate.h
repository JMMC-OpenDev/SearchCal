#ifndef miscPrivate_H
#define miscPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscPrivate.h,v 1.2 2004-07-09 14:28:55 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     24-Jun-2004  Created
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
*
*
*******************************************************************************/

/**
 * \file
 * Private misc module header file, only holding the MODULE_NAME definition.
 */

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
