#ifndef fndPrivate_H
#define fndPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: fndPrivate.h,v 1.2 2004-09-22 13:39:53 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* sccmgr    22-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * Private header file of the fnd library.
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
#define MODULE_ID "fnd"

 

#ifdef __cplusplus
}
#endif


#endif /*!fndPrivate_H*/

/*___oOo___*/
