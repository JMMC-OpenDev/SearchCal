#ifndef miscString_H
#define miscString_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
extern "C"
{
#endif


/* 
 * MCS Headers
 */
#include "mcs.h"


/* Remove leading / trailing white spaces */
#define TRIM_SPACE(str) \
    miscTrimString(str, " ");

/*
 * Pubic functions declaration
 */

mcsCOMPL_STAT miscStripQuotes    (char*               str);

void          miscTrimString     (char*               str,
                                  const char*         trimChars);

mcsCOMPL_STAT miscStrToUpper     (char*               str);

mcsLOGICAL    miscIsSpaceStr     (const char*         str);

mcsLOGICAL    miscIsCommentLine  (const char*          line,
                                  const char* /* 4 */  commentPatternStr);

mcsCOMPL_STAT miscReplaceChrByChr(char*               str,
                                  const char          originalChar,
                                  const char          newChar);

mcsCOMPL_STAT miscReplaceNonAlphaNumericChrByChr(char*      str,
                                                 const char newChar);

mcsCOMPL_STAT miscDeleteChr      (char*               str,
                                  const char          searchedChar,
                                  const mcsLOGICAL    allFlag);

char*         miscDuplicateString(const char*         str);

mcsCOMPL_STAT miscSplitString    (const char*         str,
                                  const char          delimiter,
                                  mcsSTRING256        subStrArray[],
                                  const mcsUINT32     maxSubStrNb,
                                  mcsUINT32*          subStrNb);

mcsCOMPL_STAT miscSplitStringDyn (const char*         str,
                                  const char          delimiter,
                                  char* /* DYN */     subStrArray[],
                                  const mcsUINT32     maxSubStrLen[],
                                  const mcsUINT32     maxSubStrNb,
                                  mcsUINT32*          subStrNb);

#ifdef __cplusplus
}
#endif

#endif /*!miscString_H*/

/*___oOo___*/
