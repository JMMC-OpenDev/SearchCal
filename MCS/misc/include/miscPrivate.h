#ifndef miscPrivate_H
#define miscPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscPrivate.h,v 1.5 2004-12-17 08:15:04 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* gzins     24-Jun-2004  Created
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
* lafrasse  23-Jul-2004  Added miscDYN_BUF_MAGIC_STRUCTURE_ID from miscDynBuf.h
* lafrasse  02-Aug-2004  Added mcs.h include
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

/* 
 * MCS Headers
 */
#include "mcs.h"


/* Module name */ 
#define MODULE_ID "misc"

/**
 * Unic MCS structure identifier.
 *
 * It is meant to allow the testing of a Dynamic Buffer struture initialization
 * state (weither it has allready been initialized as a miscDYN_BUF or not).
 */
#define miscDYN_BUF_MAGIC_STRUCTURE_ID ((mcsUINT32) 2813741963u)

#ifdef __cplusplus
}
#endif

#endif /*!miscPrivate_H*/
/*___oOo___*/
