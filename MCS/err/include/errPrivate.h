#ifndef errPrivate_H
#define errPrivate_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: errPrivate.h,v 1.1 2004-06-21 17:09:47 gzins Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
*
*
*******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern C {
#endif

/* Module name */
#define MODULE_ID "err"

/* Max size of the error message */
#define errMSG_MAX_LEN 128

#ifdef __cplusplus
}
#endif

#endif /*!errPrivate_H*/
/*___oOo___*/
