#ifndef miscFile_H
#define miscFile_H
/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscFile.h,v 1.15 2010-01-15 14:18:44 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2005/10/10 12:00:11  lafrasse
 * Added miscLocateDir()
 *
 * Revision 1.13  2005/10/06 15:12:46  lafrasse
 * Added miscGetEnvVarIntValue function
 *
 * Revision 1.12  2005/05/20 16:22:50  lafrasse
 * Code review : refined user and developper documentation, functions reordering, and rationnalized miscYankExtension()
 *
 * Revision 1.11  2005/04/06 09:31:50  gluck
 * Code review: minor changes
 *
 * Revision 1.10  2005/02/15 09:37:52  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  07-Oct-2004  Changed miscFileExists API
 * lafrasse  01-Oct-2004  Changed miscResolvePath API for consistency
 * lafrasse  30-Sep-2004  Added miscLocateFile
 * lafrasse  27-Sep-2004  Added miscLocateFileInPath
 * lafrasse  25-Sep-2004  Added miscFileExists
 * lafrasse  23-Aug-2004  Changed miscGetEnvVarValue API
 * lafrasse  02-Aug-2004  Forked from misc.h to isolate miscFile headers
 *                        Moved mcs.h include in from miscFile.c
 *
 ******************************************************************************/

/**
 * \file
 * Declaration of miscFile functions.
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
 
mcsCOMPL_STAT miscGetEnvVarValue    (const char       *envVarName,
                                     char             *envVarValueBuffer,
                                     mcsUINT32         envVarValueBufferLength);

mcsCOMPL_STAT miscGetEnvVarIntValue (const char       *envVarName,
                                     mcsINT32         *envVarIntValue);

char *        miscGetFileName       (const char       *fullPath);

char *        miscGetExtension      (char             *fullPath);

mcsCOMPL_STAT miscYankExtension     (char             *fullPath,
                                     char             *extension);

mcsCOMPL_STAT miscYankLastPath      (char             *path);

char*         miscResolvePath       (const char       *orginalPath);

mcsLOGICAL    miscFileExists        (const char       *fullPath,
                                     mcsLOGICAL        addError);

char*         miscLocateFileInPath  (const char       *path,
                                     const char       *fileName);

char*         miscLocateFile        (const char       *fileName);

char*         miscLocateDir         (const char       *dirName);

char*         miscLocateExe         (const char       *exeName);

#ifdef __cplusplus
}
#endif

#endif /*!miscFile_H*/

/*___oOo___*/
