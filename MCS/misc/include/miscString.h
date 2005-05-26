#ifndef miscString_H
#define miscString_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscString.h,v 1.12 2005-05-26 08:48:20 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/02/25 16:43:52  lafrasse
 * Added miscDeleteChr()
 *
 * Revision 1.10  2005/02/21 15:27:52  lafrasse
 * Added miscIsCommentLine()
 *
 * Revision 1.9  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  17-Jan-2005  Added miscSplitString function
 * gzins     16-Dec-2004  Added miscDuplicateString function
 * gzins     15-Dec-2004  Added miscTrimString function
 * lafrasse  02-Aug-2004  Forked from misc.h to isolate miscString headers
 *                        Moved mcs.h include in from miscString.c
 *
 ******************************************************************************/

/**
 * \file
 * Definition of miscString functions.
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
 
mcsCOMPL_STAT miscStripQuotes    (      char         *str);

mcsCOMPL_STAT miscTrimString     (      char         *str,
                                  const char         *trimChars);

mcsCOMPL_STAT miscStrToUpper     (      char         *str);

mcsLOGICAL    miscIsSpaceStr     (const char         *str);

mcsLOGICAL    miscIsCommentLine  (const char         *line,
                                  const mcsSTRING4    commentPatternStr);

mcsCOMPL_STAT miscReplaceChrByChr(      char         *str,
                                  const char          originalChar,
                                  const char          newChar);

mcsCOMPL_STAT miscDeleteChr      (      char         *str,
                                  const char          searchedChar,
                                  const mcsLOGICAL    allFlag);

char         *miscDuplicateString(const char         *str);

mcsCOMPL_STAT miscSplitString    (const char         *str,
                                  const char          delimiter,
                                        mcsSTRING256  subStrArray[],
                                  const mcsUINT32     maxSubStrNb,
                                        mcsUINT32    *subStrNb);

#ifdef __cplusplus
}
#endif

#endif /*!miscString_H*/

/*___oOo___*/
