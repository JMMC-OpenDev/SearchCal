#ifndef misc_H
#define misc_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: misc.h,v 1.12 2004-07-22 13:23:45 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
* lafrasse  17-Jun-2004  added miscGetLocalTimeStr
*                        added miscStripQuotes
*                        added miscStrToUpper
*                        added miscGetExtension
* lafrasse  18-Jun-2004  added miscYankExtension
*                        added miscResolvePath
* lafrasse  23-Jun-2004  added miscDynBuf stuff
* lafrasse  01-Jul-2004  moved miscDynBuf stuff to miscDynBuf.h
* lafrasse  08-Jul-2004  Added 'modc' like doxygen documentation tags
* lafrasse  19-Jul-2004  Added miscDynStr doxygen comment
* lafrasse  20-Jul-2004  Added miscResolvePath, miscGetValueEnvVar, and
*                        miscYankLastPath from miscFile.c
* lafrasse  22-Jul-2004  Added an include of misDynStr.h to centralize user
*                        includes on misc.h
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
extern C {
#endif

#include "miscDynStr.h"

/*
 * Pubic functions declaration
 */
 
char *        miscGetFileName    (char *fullPath);
char *        miscGetExtension   (char *fullPath);
mcsCOMPL_STAT miscYankExtension  (char *fullPath, char *extension);
mcsCOMPL_STAT miscResolvePath    (const char *orginalPath, char **resolvedPath);
mcsCOMPL_STAT miscGetEnvVarValue (const char *envVarName, char **envVarValue);
mcsCOMPL_STAT miscYankLastPath   (char *path);

void miscGetUtcTimeStr           (mcsBYTES32 localTime, mcsINT32 precision);
void miscGetLocalTimeStr         (mcsBYTES32 localTime, mcsINT32 precision);

void miscStripQuotes             (char *string);
void miscStrToUpper              (char *string);

#ifdef __cplusplus
}
#endif

#endif /*!misc_H*/

/*___oOo___*/
