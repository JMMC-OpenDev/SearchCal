#ifndef misc_H
#define misc_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: misc.h,v 1.16 2004-07-23 09:14:11 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  Created
* lafrasse  17-Jun-2004  Added miscGetLocalTimeStr, miscStripQuotes,
*                        miscStrToUpper and miscGetExtension
* lafrasse  18-Jun-2004  Added miscYankExtension and miscResolvePath
* lafrasse  23-Jun-2004  added miscDynBuf stuff
* lafrasse  01-Jul-2004  moved miscDynBuf stuff to miscDynBuf.h
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
* lafrasse  19-Jul-2004  Added miscDynStr doxygen comment
* lafrasse  20-Jul-2004  Added miscResolvePath, miscGetValueEnvVar, and
*                        miscYankLastPath for miscFile.c
* lafrasse  22-Jul-2004  Added an include of misDynStr.h to centralize 'misc'
*                        user includes on misc.h
*                        Added error management to miscDate
* gzins     23-Jul-2004  Added miscIsSpaceStr to miscString
* lafrasse  23-Jul-2004  Added error management to miscString
*
*
*******************************************************************************/
 
/**
 * \file
 * This header contains all the miscDate, miscFile and miscString functions
 * declarations, but NOT those of miscDynBuf nor those of miscDynStr.
 *
 * miscDynBuf and miscDynStr functions (due to their number), have their own
 * include file, respectivally called miscDynBuf.h and miscDynStr.h.
 */

/* The following piece of code alternates the linkage type to C for all 
functions declared within the braces, which is necessary to use the 
functions in C++-code.
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "miscDynStr.h"

/*
 * Pubic functions declaration
 */
 
/* miscDate stuff */
mcsCOMPL_STAT miscGetUtcTimeStr  (mcsBYTES32 localTime, mcsINT32 precision);
mcsCOMPL_STAT miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision);


/* miscFile stuff */
char *        miscGetFileName    (char *fullPath);
char *        miscGetExtension   (char *fullPath);
mcsCOMPL_STAT miscYankExtension  (char *fullPath, char *extension);
mcsCOMPL_STAT miscResolvePath    (const char *orginalPath, char **resolvedPath);
mcsCOMPL_STAT miscGetEnvVarValue (const char *envVarName, char **envVarValue);
mcsCOMPL_STAT miscYankLastPath   (char *path);


/* miscString stuff */
mcsCOMPL_STAT miscStripQuotes    (char *string);
mcsCOMPL_STAT miscStrToUpper     (char *string);
mcsLOGICAL    miscIsSpaceStr     (char *string);

#ifdef __cplusplus
}
#endif

#endif /*!misc_H*/

/*___oOo___*/
