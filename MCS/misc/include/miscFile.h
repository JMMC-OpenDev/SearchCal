#ifndef miscFile_H
#define miscFile_H
/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscFile.h,v 1.4 2004-09-27 14:59:47 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  02-Aug-2004  Forked from misc.h to isolate miscFile headers
*                        Moved mcs.h include in from miscFile.c
* lafrasse  23-Aug-2004  Changed miscGetEnvVarValue API
* lafrasse  25-Sep-2004  Added miscFileExists
*
*
*******************************************************************************/

/**
 * \file
 * This header contains ONLY the miscFile functions declarations.
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
 
char *        miscGetFileName    (char *fullPath);
char *        miscGetExtension   (char *fullPath);
mcsCOMPL_STAT miscYankExtension  (char *fullPath, char *extension);
mcsCOMPL_STAT miscResolvePath    (const char *orginalPath, char **resolvedPath);
mcsCOMPL_STAT miscGetEnvVarValue (const char *envVarName,
                                  char *envVarValueBuffer,
                                  mcsUINT32 envVarValueBufferLength);
mcsCOMPL_STAT miscYankLastPath   (char *path);
mcsCOMPL_STAT miscFileExists     (const char *fullPath);


#ifdef __cplusplus
}
#endif

#endif /*!miscFile_H*/

/*___oOo___*/
