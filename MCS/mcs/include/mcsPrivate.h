#ifndef mcsPrivate_H
#define mcsPrivate_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: mcsPrivate.h,v 1.2 2005-01-28 18:50:44 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * gzins     15-Jun-2004  created
 *
 *
 ******************************************************************************/

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/

#ifdef __cplusplus
extern C {
#endif

/* Module name */
#define MODULE_ID   "mcs"

#ifdef __cplusplus
}
#endif

#endif /*!mcsPrivate_H*/


/*___oOo___*/
