#ifndef envPrivate_H
#define envPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: envPrivate.h,v 1.2 2004-12-07 16:45:56 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  07-Dec-2004  Created
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

 
/**
 * Default message Manager port number for connection
 */
#define envDEFAULT_MESSAGE_MANAGER_PORT_NUMBER 1991


#ifdef __cplusplus
}
#endif


#endif /*!envPrivate_H*/

/*___oOo___*/
