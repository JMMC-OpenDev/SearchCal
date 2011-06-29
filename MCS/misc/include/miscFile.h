#ifndef miscFile_H
#define miscFile_H
/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
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
 
mcsCOMPL_STAT miscGetEnvVarValue    (const char*  envVarName,
                                     char*        envVarValueBuffer,
                                     mcsUINT32    envVarValueBufferLength);

mcsCOMPL_STAT miscGetEnvVarIntValue (const char*  envVarName,
                                     mcsINT32*    envVarIntValue);

char*         miscGetFileName       (const char*  fullPath);

char*         miscGetExtension      (const char*  fullPath);

mcsCOMPL_STAT miscYankExtension     (char*        fullPath,
                                     const char*  extension);

mcsCOMPL_STAT miscYankLastPath      (char*        path);

char*         miscResolvePath       (const char*  orginalPath);

mcsLOGICAL    miscFileExists        (const char*  fullPath,
                                     mcsLOGICAL   addError);

char*         miscLocateFileInPath  (const char*  path,
                                     const char*  fileName);

char*         miscLocateFile        (const char*  fileName);

char*         miscLocateDir         (const char*  dirName);

char*         miscLocateExe         (const char*  exeName);

#ifdef __cplusplus
}
#endif

#endif /*!miscFile_H*/

/*___oOo___*/
