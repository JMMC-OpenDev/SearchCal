#ifndef cmdPrivate_H
#define cmdPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdPrivate.h,v 1.2 2005-01-12 08:44:22 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* sccmgr    19-Nov-2004  Created
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
#define MODULE_ID "cmd"

/*
 * Max length for short description.
 */
#define SHORT_DESC_MAX_LEN 60


#ifdef __cplusplus
}
#endif


#endif /*!cmdPrivate_H*/

/*___oOo___*/
