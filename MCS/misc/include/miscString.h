#ifndef miscString_H
#define miscString_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscString.h,v 1.7 2005-01-19 10:29:42 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  02-Aug-2004  Forked from misc.h to isolate miscString headers
*                        Moved mcs.h include in from miscString.c
* gzins     15-Dec-2004  Added miscTrimString function
* gzins     16-Dec-2004  Added miscDuplicateString function
* lafrasse  17-Jan-2005  Added miscSplitString function
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
 * MCS Headers
 */
#include "mcs.h"


/*
 * Pubic functions declaration
 */
 
mcsCOMPL_STAT miscStripQuotes    (char               *string);

mcsCOMPL_STAT miscTrimString     (char               *string,
                                  char               *trimChars);

mcsCOMPL_STAT miscStrToUpper     (char               *string);

mcsLOGICAL    miscIsSpaceStr     (char               *string);

mcsCOMPL_STAT miscReplaceChrByChr(char               *string,
                                  char                originalChar,
                                  char                newChar);

char         *miscDuplicateString(const char         *string);

mcsCOMPL_STAT miscSplitString    (const char         *string,
                                  const char          delimiter,
                                        mcsSTRING256  subStrings[],
                                  const mcsUINT32     maxSubStringNb,
                                        mcsUINT32    *subStringNb);

#ifdef __cplusplus
}
#endif

#endif /*!miscString_H*/

/*___oOo___*/
