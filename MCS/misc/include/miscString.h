#ifndef miscString_H
#define miscString_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscString.h,v 1.11 2005-02-25 16:43:52 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.10  2005/02/21 15:27:52  lafrasse
 * Added miscIsCommentLine()
 *
 * Revision 1.9  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  02-Aug-2004  Forked from misc.h to isolate miscString headers
 *                        Moved mcs.h include in from miscString.c
 * gzins     15-Dec-2004  Added miscTrimString function
 * gzins     16-Dec-2004  Added miscDuplicateString function
 * lafrasse  17-Jan-2005  Added miscSplitString function
 *
 ******************************************************************************/

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
 
mcsCOMPL_STAT miscStripQuotes    (      char         *string);

mcsCOMPL_STAT miscTrimString     (      char         *string,
                                        char         *trimChars);

mcsCOMPL_STAT miscStrToUpper     (      char         *string);

mcsLOGICAL    miscIsSpaceStr     (const char         *string);

mcsLOGICAL    miscIsCommentLine  (const char         *line,
                                  const mcsSTRING4    commentPattern);

mcsCOMPL_STAT miscReplaceChrByChr(      char         *string,
                                        char          originalChar,
                                        char          newChar);

mcsCOMPL_STAT miscDeleteChr      (      char         *string,
                                  const char          searchedChar,
                                  const mcsLOGICAL    allFlag);

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
