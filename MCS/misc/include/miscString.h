#ifndef miscString_H
#define miscString_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscString.h,v 1.1 2004-08-02 14:08:46 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  02-Aug-2004  Forked from misc.h to isolate miscString headers
*
*
*******************************************************************************/

/**
 * \file
 * This header contains ONLY the miscString functions declarations.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif


/*
 * Pubic functions declaration
 */
 
mcsCOMPL_STAT miscStripQuotes    (char *string);
mcsCOMPL_STAT miscStrToUpper     (char *string);
mcsLOGICAL    miscIsSpaceStr     (char *string);


#ifdef __cplusplus
}
#endif

#endif /*!miscString_H*/

/*___oOo___*/
