/*******************************************************************************
 * JMMC project
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.28  2005/02/17 14:32:42  gzins
 * Improved intialisation of static dynamic buffers
 *
 * Revision 1.27  2005/02/12 14:46:09  gzins
 * Updated miscLocateFile function:
 *  - added test of file existence before looking for in path list.
 *  - returned a resolved path
 *
 * Revision 1.26  2005/02/09 06:27:35  gzins
 * Fixed minor bug in miscFileExists; some errors was added even if addError was set to false
 *
 * Revision 1.25  2005/02/07 14:41:47  lafrasse
 * Changed miscLocateFileInPath() error management behavior in order to report only one error (and not one for each directory) if the given file was not found in all the path directories
 *
 * Revision 1.24  2005/01/28 18:39:10  gzins
 * Changed FAILURE/SUCCESS to mcsFAILURE/mscSUCCESS
 *
 * gzins     16-Jun-2004  Created from VLT file 'slxUtils.c'
 * lafrasse  17-Jun-2004  Added miscGetExtension
 * lafrasse  18-Jun-2004  Debugged miscGetExtension
 *                        Added miscYankExtension
 * lafrasse  20-Jul-2004  Added miscResolvePath, miscGetEnvVarValue, and
 *                        miscYankLastPath
 * lafrasse  23-Jul-2004  Added error management code optimisation
 * lafrasse  02-Aug-2004  Changed includes to isolate miscFile headers from
 *                        misc.h
 *                        Moved mcs.h include to miscFile.h
 *                        Changed includes due to null-terminated string
 *                        specific functions move from miscDynStr.h to
 *                        miscDynBuf.h
 * lafrasse  03-Aug-2004  Corrected a bug in miscResolvePath that was causing an
 *                        '\' append at the end of the computed path
 * lafrasse  23-Aug-2004  Changed miscGetEnvVarValue API
 * lafrasse  25-Sep-2004  Added miscFileExists
 * lafrasse  27-Sep-2004  Added miscLocateFileInPath, corrected a bug in the
 *                        miscResolvePath use of misFileExists, and refined the
 *                        doxygen documentation
 * lafrasse  30-Sep-2004  Added miscLocateFile
 * lafrasse  01-Oct-2004  Changed miscResolvePath API for consistency
 * lafrasse  07-Oct-2004  Changed miscFileExists API
 * mella     10-Nov-2004  Added xsd and xsl into pathSearchList
 * gzins     10-Dec-2004  Fixed pathSearchList for .cdf files
 *
 *----------------------------------------------------------------------------*/

/**
 * \file
 * Contains all the 'misc' Unix file path related functions definitions.
 */

static char *rcsId="@(#) $Id: miscFile.c,v 1.29 2005-03-08 07:17:04 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

/* 
 * MCS Headers
 */
#include "err.h"

/* 
 * Local Headers
 */
#include "miscFile.h"
#include "miscPrivate.h"
#include "miscErrors.h"
#include "miscDynBuf.h"

/* 
 * Local Variables and Defines
 */
#define miscEXT_IDX   0
#define miscPATH_IDX  1
static char *pathSearchList[][2] = {
   {"cfg", "../config:$INTROOT/config:$MCSROOT/config"},
   {"cdf", "../config:$INTROOT/config:$MCSROOT/config"},
   {"xsd", "../config:$INTROOT/config:$MCSROOT/config"},
   {"xsl", "../config:$INTROOT/config:$MCSROOT/config"},
   {"xml", "../errors:$INTROOT/errors:$MCSROOT/errors"},
   {NULL, NULL }
};

/**
 * Return the file name from a full path.
 * 
 * This function returns the file name with any leading directory components
 * removed. For example, miscGetFileName("../data/myFile.fits") will return
 * "myFile.fits".
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !\n\n
 *
 * \param fullPath a null-terminated string containing the full path.
 *
 * \return a null-terminated string containing the file name, or NULL.
 */
char *miscGetFileName(char *fullPath)
{
    static char *buffer = NULL;
    char *token;
    char *fileName = fullPath;

    /* If full file name is empty */
    if ((fullPath == NULL) || (strlen(fullPath) == 0))
    {
        /* Return NULL string */
        return ((char *)NULL);
    }

    /* (Re)-allocate memory for temporary buffer strtok() function modifies
     * the input string and it is needed to make a copy of the buffer before
     * using it */
    if (buffer != NULL)
    {
        free(buffer);
    }

    /* Allocate memory for buffer */
    buffer = malloc(strlen(fullPath) + 1);
    if (buffer == NULL)
    {
        errAdd(miscERR_ALLOC);
        return ((char *)NULL);
    }

    /* Copy full file name into temporary buffer */
    if (strcpy(buffer, fullPath) == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "strcpy");
        return ((char*)NULL);
    }

    /* Establish string and get the first token: */
    token = strtok( buffer, "/" );

    /* While there are tokens in "string" */       
    while( token != NULL )
    {
        /* Get next token: */
        fileName = token;

        token = strtok( NULL, "/" );   
    }

    return fileName;
}

/**
 * Return the file extension from a full path.
 * 
 * This function returns the extention of the given file name, i.e. the
 * characters after the last dot in the file name. For example,
 * miscGetExtension("../data/myFile.fits") will return "fits".
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !
 * NULL is returned when there is no dot in the file name or the last
 * dot is the first character of the file name, e.g.:
 *              "/data/.dt"
 *              "/data/.dt/cache"\n\n
 *
 * \param fullPath a null-terminated string containing the full path.
 *
 * \return a null-terminated string containing the file extension, or NULL.
 */
char *miscGetExtension(char *fullPath)
{
    char *chrPtr, *chrPtr2;

    /* Makes chrPtr points to the last occurence of '.' */
    if ((chrPtr = strrchr(fullPath, '.')) == NULL)
    {
        /* Exits if no extension found */
        return ((char*)NULL);
    }

    /* Check that the 'extension' found is not part of the filepath
     * like e.g.: "/data/.dt/cache"
     *
     * Check also the following: "/data/.dt"
     *
     * In those cases there is no extension.
     */

    /* Makes chrPtr2 points to the last occurence of '/' */
    chrPtr2 = strrchr(fullPath, '/');

    if (chrPtr2 > chrPtr) /* "/data/.dt/cache" */
    {
        return ((char*)NULL);
    }

    if (*(chrPtr - 1) == '/') /* "/data/.dt" */
    {
        return ((char*)NULL);
    }

    return (chrPtr + 1);
}

/**
 * Give back a full path without its file extension.
 * 
 * This function cats down the given file extension off from the file name
 * using the same original buffer.
 * Note that when filename does not end with the given extention the function
 * has no effect.
 *
 * The extention can be given with or without the dots, e.g.:  "fits", or
 * ".fits".
 *
 * If \em the extension is a null pointer, this function cats down the
 * extention, found using miscGetExtension(), of the given 'fileName' if it has
 * any extention.
 *
 * For example, miscYankExtension("../data/myFile.fits") will give back 
 * "../data/myFile".
 *
 * \warning When an extension is specified, the function looks for the FIRST
 * occurance of the \em extention in the \em fileName ! Therefore if the given
 * file name is :'file.fitsname.fits' and the extention is 'fits' the
 * resulting file name is :'file'.\n\n
 *
 * \param fullPath a null-terminated string containing a full path
 * \param extension a null-terminated string containing the file extension
 * to yank
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscYankExtension(char *fullPath, char *extension)
{    
    int      pos;
    char     *extPtr;

    /* return if the file name is null */
    if (fullPath == NULL)
    {
        return mcsFAILURE;
    }

    /* if extension is specified */
    if (extension != NULL)
    {
        mcsSTRING32 extLoc;

        if (*extension != '.')
        {
            sprintf(extLoc, ".%s", extension);
        }
        else
        {
            sprintf(extLoc, "%s", extension);
        }

        if ((extPtr = strstr(fullPath, extLoc)) != NULL)
        {
            *extPtr = '\0';
        }
    }
    else
    {
        /* get the extention */
        if ((extPtr = miscGetExtension(fullPath)) == NULL)
        {
            return mcsFAILURE;
        }
        else
        {
            /* find the position of the extension */
            pos = strlen(fullPath)-strlen(extPtr)-1;

            /* cut the string there */
            fullPath[pos] = '\0';
        }
    }

    return mcsSUCCESS;
}

/**
 * Return a resolved path of entries that may be environment variables and in
 * direct reference to HOME.
 *
 * The function can resolve "~/$MY_VAR/MY_DIR/file", "~<user>/MY_DIR" or
 * "$HOME/$MY_VAR/MY_DIR/file". A colon separated list of variables will be
 * resolved as well. Note that it can resolve pathes that starts with or
 * includes patterns: ./ or ../
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !
 * No space is allowed in the input path name.\n\n
 *
 * \param unresolvedPath a null-terminated string pointer to the path to be
 * resolved
 *
 * \return a pointer to the resolved path, or NULL
 */
char*         miscResolvePath    (const char *unresolvedPath)
{
    static mcsLOGICAL   init = mcsFALSE;
    static miscDYN_BUF  builtPath;
    miscDYN_BUF         pathToResolve;
    mcsSTRING256        tmpPath, tmpEnvVar;
    mcsINT32            length;

    /* Check parameter */
    if (unresolvedPath == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "unresolvedPath"); 
        return NULL;
    }

    /* Initialize buffer (if not already done */
    if (init == mcsFALSE)
    {
        miscDynBufInit(&builtPath);
        init = mcsTRUE;
    }

    /* Reset the static Dynamic Buffer */
    if (miscDynBufReset(&builtPath) == mcsFAILURE)
    {
        return NULL;
    }
    if (miscDynBufInit(&pathToResolve) == mcsFAILURE)
    {
        return NULL;
    }
    miscDynBufAppendString(&pathToResolve, unresolvedPath);

    char *leftToBeResolvedPathPtr  = (char*)miscDynBufGetBuffer(&pathToResolve);
    char *nextSlashPtr             = strchr(leftToBeResolvedPathPtr, '/');
    do
    {
        if (*leftToBeResolvedPathPtr == '$')
        {
            if (nextSlashPtr != NULL)
            {
                length = ((nextSlashPtr - leftToBeResolvedPathPtr) - 1);
            }
            else
            {
                length = strlen(leftToBeResolvedPathPtr);
            }

            if (strncpy(tmpPath, (leftToBeResolvedPathPtr + 1), length) == NULL)
            {
                errAdd(miscERR_FUNC_CALL, "strncpy");
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }

            *(tmpPath + length) = '\0';

            if (miscGetEnvVarValue(tmpPath, tmpEnvVar, sizeof(mcsSTRING256))
                == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }

            if (miscDynBufAppendString(&builtPath, tmpEnvVar) == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }
        }
        else if (*leftToBeResolvedPathPtr == '~')
        {
            /* Path of the format: "~/MY_DIR/" or "~<user>/" */
            memcpy(tmpPath, "HOME", 5);

            if (miscGetEnvVarValue(tmpPath, tmpEnvVar, sizeof(mcsSTRING256))
                == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }

            if (miscDynBufAppendString(&builtPath, tmpEnvVar) == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }
        }
        else
        {
            if (nextSlashPtr != NULL)
            {
                length = (nextSlashPtr - leftToBeResolvedPathPtr);
            }
            else
            {
                length = strlen(leftToBeResolvedPathPtr);
            }

            if (strncpy(tmpPath, leftToBeResolvedPathPtr, length) == NULL)
            {
                errAdd(miscERR_FUNC_CALL, "strncpy");
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }

            *(tmpPath + length) = '\0';

            if (miscDynBufAppendString(&builtPath, tmpPath) == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }
        }
        if (miscDynBufAppendString(&builtPath, "/") == mcsFAILURE)
        {
            miscDynBufDestroy(&pathToResolve);
            return NULL;
        }


        if (nextSlashPtr != NULL)
        {
            if (*nextSlashPtr != '\0')
            {
                leftToBeResolvedPathPtr = (nextSlashPtr + 1);
            }
            else
            {
                leftToBeResolvedPathPtr = nextSlashPtr;
            }
        }
        else
        {
            leftToBeResolvedPathPtr = "\0";
        }

        if (*leftToBeResolvedPathPtr == ':')
        {
            if (miscDynBufAppendString(&builtPath, ":") == mcsFAILURE)
            {
                miscDynBufDestroy(&pathToResolve);
                return NULL;
            }

            leftToBeResolvedPathPtr++;
        }
    }
    while (((nextSlashPtr = strchr(leftToBeResolvedPathPtr, '/')) != NULL)
           || (*leftToBeResolvedPathPtr != '\0'));

    /* Since we cannot know if a filename is contained in the path, we should
     * not allow slash in the end of the complete path
     */
    /* Get the Dynamic Buffer length */
    mcsUINT32 builtPathLength = 0;
    if (miscDynBufGetNbStoredBytes(&builtPath, &builtPathLength) == mcsFAILURE)
    {
        miscDynBufDestroy(&pathToResolve);
        return NULL;
    }

    /* Get Dynamic Buffer internal buffer pointer */
    char *endingChar = NULL;
    if ((endingChar = miscDynBufGetBuffer(&builtPath)) == NULL)
    {
        miscDynBufDestroy(&pathToResolve);
        return NULL;
    }
    
    /* Compute the last path character position */
    endingChar += (builtPathLength - 2);

    /* If the last path charater is an '/' */
    if (*endingChar == '/')
    {
        /* Replace with a '\0' */
        *endingChar = '\0';

        /* Decrease its length */
        builtPath.storedBytes--;
    }

    /* Strip the Dynamic Buffer */
    if (miscDynBufStrip(&builtPath) == mcsFAILURE)
    {
        miscDynBufDestroy(&pathToResolve);
        return NULL;
    }
    
    miscDynBufDestroy(&pathToResolve);
    return miscDynBufGetBuffer(&builtPath);
}

/**
 * Give back the value of a specified Environment Variable.
 *
 * \param envVarName a null-terminated string containing the searched
 * Environment Variable name \em without the '$'
 * \param envVarValueBuffer an already allocated buffer to receive the
 * Environment Variable value
 * \param envVarValueBufferLength the length of the already allocated buffer
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscGetEnvVarValue (const char *envVarName,
                                  char *envVarValueBuffer,
                                  mcsUINT32 envVarValueBufferLength)
{
    char               *chrPtr;

    /* Return if the anv. var. name is null */
    if (envVarName == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "envVarName");
        return mcsFAILURE;
    }

    if ((chrPtr = getenv(envVarName)) == NULL)
    {
        errAdd(miscERR_FILE_ENV_VAR_NOT_DEF, envVarName);
        return mcsFAILURE;
    }

    if (strlen(chrPtr) >= envVarValueBufferLength)
    {
        errAdd(miscERR_FILE_ENV_VAR_TOO_LONG, envVarName);
        return mcsFAILURE;
    }

    strncpy(envVarValueBuffer, chrPtr, envVarValueBufferLength);

    return mcsSUCCESS;
}

/**
 * Remove the last path or filename in a given path.
 *
 * \param path a null-terminated string containing the path to be yanked
 *
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscYankLastPath(char *path)
{
    char *chrPtr = NULL;

    if ((chrPtr = strrchr(path, '/')) == NULL)
    {
        /* There is no '/' in the received path */
        return mcsSUCCESS;
    }

    *chrPtr = '\0';

    return mcsSUCCESS;
}

/**
 * Test if a file exists at a given path.
 *
 * \param fullPath a null-terminated string containing the path to be tested
 * \param addError an mcsLOGICAL to specify weither or not this function should
 * raise an error that tries to explain the reason why the file was not found
 *
 * \return TRUE if the file exists, FALSE otherwise
 */
mcsLOGICAL    miscFileExists        (const char       *fullPath,
                                     mcsLOGICAL        addError)
{
    /* Test the fullPath parameter validity */
    if ((fullPath == NULL) || (strlen(fullPath) == 0))
    {
        if (addError == mcsTRUE)
        {
            errAdd(miscERR_NULL_PARAM, "fullPath");
        }
        return mcsFALSE;
    }

    /* Resolve any Env. Var contained in the given path */
    char* resolvedPath =  miscResolvePath(fullPath);
    if ( resolvedPath == NULL)
    {
        if (addError == mcsFALSE)
        {
            errResetStack();
        }
        return mcsFALSE;
    }

    /* Get file system informations of the file to be tested */
    struct stat fileInformationBuffer;
    if (stat(resolvedPath, &fileInformationBuffer) == -1)
    {
        /* If an explaining error should be raised */
        if (addError == mcsTRUE)
        {
            /* Raise an error according to the problem detected by the 'stat'
             * function call
             */
            switch (errno)
            {
                case EACCES:
                    /* Permission denied */
                    errAdd(miscERR_FILE_PERMISSION_DENIED, resolvedPath);
                    break;
        
                case ENAMETOOLONG:
                    /* File name too long */
                    errAdd(miscERR_FILE_NAME_TOO_LONG, resolvedPath);
                    break;
        
                case ENOENT:
                    /* A component of the path doesn't exist */
                case ENOTDIR:
                    /* A component of the path is not a directory */
                    errAdd(miscERR_FILE_DOESNT_EXIST, resolvedPath);
                    break;
        
                case ELOOP:
                    /* Too many sym. links encountered while traversing the path
                     */
                    errAdd(miscERR_FILE_TOO_MANY_SYM_LINKS, resolvedPath);
                    break;
        
                default : 
                    errAdd(miscERR_FILE_UNDEFINED_ERRNO, resolvedPath, errno);
            }
        }

        return mcsFALSE;
    }

    return mcsTRUE;
}

/**
 * Search for a file in a list of path.
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !\n\n
 *
 * \param path the list of path to be searched, each separated by colons (':')
 * \param fileName the seeked file name
 *
 * \return a pointer to the \em first path where the file is, or NULL if not
 * found
 */
char* miscLocateFileInPath(const char *path, const char *fileName)
{
    static mcsLOGICAL  init = mcsFALSE;
    static miscDYN_BUF tmpPath;
    const char* originalPath = path;

    /* Initialize buffer (if not already done */
    if (init == mcsFALSE)
    {
        miscDynBufInit(&tmpPath);
        init = mcsTRUE;
    }

    /* Test the path parameter validity */
    if ((path == NULL) || (strlen(path) == 0))
    {
        errAdd(miscERR_NULL_PARAM, "path");
        return NULL;
    }

    /* Test the fileName parameter validity */
    if ((fileName == NULL) || (strlen(fileName) == 0))
    {
        errAdd(miscERR_NULL_PARAM, "fileName");
        return NULL;
    }

    /* Reset the static Dynamic Buffer */
    if (miscDynBufReset(&tmpPath) == mcsFAILURE)
    {
        return NULL;
    }

    /*
     * For each path part, until all of them were tested or a valid path was
     * found
     */
    char *validPath = NULL;
    do
    {
        /* Compute the length of the current path part */
        int pathPartLength = 0;
        char *colonPtr = strchr(path, ':');
        if (colonPtr == NULL)
        {
            pathPartLength = strlen(path);
        }
        else
        {
            pathPartLength = colonPtr - path;
        }

        /* Construct the to-be-tested temporary path */
        miscDynBufAppendBytes(&tmpPath, (char*)path, pathPartLength);
        miscDynBufAppendBytes(&tmpPath, "/", 1);
        miscDynBufAppendString(&tmpPath, (char*)fileName);

        /* If no file exists at the temporary path */
        validPath = miscDynBufGetBuffer(&tmpPath);
        if (miscFileExists(validPath, mcsFALSE) == mcsFALSE)
        {
            /* Reset the temporary path variable */
            validPath = NULL;

            /* Reset the static Dynamic Buffer */
            if (miscDynBufReset(&tmpPath) == mcsFAILURE)
            {
                return NULL;
            }

            /* If there is any ':' left in the given path */
            path = strchr(path, ':');
            if (path != NULL)
            {
                /* Make path pointer point to the next part beginning */
                path++;
            }
        }
    } while ((path != NULL) && (validPath == NULL));

    /* Minimize allocated memory used by the static Dynamic Buffer */
    if (miscDynBufStrip(&tmpPath) == mcsFAILURE)
    {
        return NULL;
    }

    /* If the file was not found along the path */
    if (validPath == NULL)
    {
        /* Add an error in the Errors Stack */
        errAdd(miscERR_FILE_NOT_FOUND_IN_PATH, fileName, originalPath);
    }

    return miscResolvePath(validPath);
}

/**
 * Search for a file according to its extension in the preconfigured
 * pathSearchList path list.
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !\n\n
 *
 * \param fileName the name of the searched file with its extension
 *
 * \return a pointer to the \em first path where the file is, or NULL if not
 * found
 */
char* miscLocateFile (const char *fileName)
{
    /* Test the fileName parameter validity */
    if ((fileName == NULL) || (strlen(fileName) == 0))
    {
        errAdd(miscERR_NULL_PARAM, "fileName");
        return NULL;
    }
    
    /*
     * Check first if the file exists; i.e if the given file corresponds
     * to an accessible file.
     */
    if (miscFileExists(fileName, mcsFALSE) == mcsTRUE)
    {
        return miscResolvePath(fileName);
    }

    /* Get the file extension */
    char* fileExtension = miscGetExtension((char*)fileName);
    if (fileExtension == NULL)
    {
        errAdd(miscERR_FILE_EXTENSION_MISSING, fileName);
        return NULL;
    }

    /*
     * For each path of the list, until all of them were tested or a path
     * corresponding to fileExtension was found
     */
    int i = 0;
    mcsLOGICAL found = mcsFALSE;
    while (pathSearchList[i][miscEXT_IDX] != NULL)
    {
        /* Compare the file extension with the current one in the path list */
        if (strcmp(fileExtension, pathSearchList[i][miscEXT_IDX]) == 0)
        {
            found = mcsTRUE;
            break;
        }
        i++;
    }
    /* Return weither the file at the path or not */
    if (found == mcsTRUE)
    {
        return miscLocateFileInPath(pathSearchList[i][miscPATH_IDX],
                                    fileName);
    }
    else
    {
        errAdd(miscERR_FILE_EXTENSION_UNKNOWN, fileExtension, fileName);
        return NULL;
    }
}


/*___oOo___*/
