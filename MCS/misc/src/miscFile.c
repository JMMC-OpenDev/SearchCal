/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
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
*                        Changed includes due to null-terminated string specific
*                        functions move from miscDynStr.h to miscDynBuf.h
* lafrasse  03-Aug-2004  Corrected a bug in miscResolvePath that was causing an
*                        '\' append at the end of the computed path
*
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Contains all the 'misc' Unix file path related functions definitions.
 */

static char *rcsId="@(#) $Id: miscFile.c,v 1.11 2004-08-03 13:48:04 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>

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
 * \param fullPath a null-terminated char array containing the full path.
 *
 * \return a null-terminated char array containing the file name, or NULL.
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

    buffer = malloc(strlen(fullPath) + 1);

    if (buffer == NULL)
    {
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
 * \param fullPath a null-terminated char array containing the full path.
 *
 * \return a null-terminated char array containing the file extension, or NULL.
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
 * \param fullPath a null-terminated char array containing a full path
 * \param extension a null-terminated char array containing the file extension
 * to yank
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscYankExtension(char *fullPath, char *extension)
{    
    int      pos;
    char     *extPtr;

    /* return if the file name is null */
    if (fullPath == NULL)
    {
        return FAILURE;
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
            return FAILURE;
        }
        else
        {
            /* find the position of the extension */
            pos = strlen(fullPath)-strlen(extPtr)-1;

            /* cut the string there */
            fullPath[pos] = '\0';
        }
    }

    return SUCCESS;
}

/**
 * Give back a resolved path of entries that may be environment variables and in
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
 * \param orginalPath a null-terminated char array containing an unresolved path
 * \param resolvedPath a null-terminated char array containing the resolved path
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscResolvePath(const char *orginalPath, char **resolvedPath)
{
    static miscDYN_BUF  builtPath;
    char                *nextSlashPtr, *leftToBeResolvedPathPtr, *tmpEnvVarPtr;
    char                *nullTerm = "\0";
    mcsSTRING256        tmpPath;
    mcsINT32            length;

    /* Try to reset the static Dynamic Buffer */
    if (miscDynBufReset(&builtPath) == FAILURE)
    {
        return FAILURE;
    }

    *resolvedPath = NULL;
    leftToBeResolvedPathPtr = (char *)orginalPath;
    nextSlashPtr = strchr(leftToBeResolvedPathPtr, '/');
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
                return FAILURE;
            }

            *(tmpPath + length) = '\0';

            if (miscGetEnvVarValue(tmpPath, &tmpEnvVarPtr) == FAILURE)
            {
                return FAILURE;
            }

            if (miscDynBufAppendString(&builtPath, tmpEnvVarPtr) == FAILURE)
            {
                return FAILURE;
            }
        }
        else if (*leftToBeResolvedPathPtr == '~')
        {
            /* Path of the format: "~/MY_DIR/" or "~<user>/" */
            if (memcpy(tmpPath, "HOME", 5) == NULL)
            {
                errAdd(miscERR_MEM_FAILURE);
                return FAILURE;
            }

            if (miscGetEnvVarValue(tmpPath, &tmpEnvVarPtr) == FAILURE)
            {
                return FAILURE;
            }

            if (miscDynBufAppendString(&builtPath, tmpEnvVarPtr) == FAILURE)
            {
                return FAILURE;
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
                return FAILURE;
            }

            *(tmpPath + length) = '\0';

            if (miscDynBufAppendString(&builtPath, tmpPath) == FAILURE)
            {
                return FAILURE;
            }
        }

        if (miscDynBufAppendString(&builtPath, "/") == FAILURE)
        {
            return FAILURE;
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
            leftToBeResolvedPathPtr = nullTerm;
        }

        if (*leftToBeResolvedPathPtr == ':')
        {
            if (miscDynBufAppendString(&builtPath, ":") == FAILURE)
            {
                return FAILURE;
            }

            leftToBeResolvedPathPtr++;
        }
    }
    while (((nextSlashPtr = strchr(leftToBeResolvedPathPtr, '/')) != NULL)
           || (*leftToBeResolvedPathPtr != '\0'));

    /* Since we cannot know if a filename is contained in the path, we should
     * not allow slash in the end of the complete path
     */
    /* Try to get the Dynamic Buffer length */
    mcsUINT32 builtPathLength = 0;
    if (miscDynBufGetStoredBytesNumber(&builtPath, &builtPathLength) == FAILURE)
    {
        return FAILURE;
    }

    /* Try to get Dynamic Buffer internal buffer pointer */
    char *endingChar = NULL;
    if ((endingChar = miscDynBufGetBufferPointer(&builtPath)) == NULL)
    {
        return FAILURE;
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

    /* Try to strip the Dynamic Buffer */
    if (miscDynBufStrip(&builtPath) == FAILURE)
    {
        return FAILURE;
    }
    
    /* Try to get Dynamic Buffer internal buffer pointer */
    if ((*resolvedPath = miscDynBufGetBufferPointer(&builtPath)) == NULL)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/**
 * Give back the value of a specified Environment Variable.
 *
 * \warning This function is \em NOT re-entrant. The returned allocated buffer
 * will be deallocated on the next call !\n\n
 *
 * \param envVarName a null-terminated char array containing the searched
 * Environment Variable name \em without the '$'
 * \param envVarValue a null-terminated char array containing the searched
 * Environment Variable value
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscGetEnvVarValue(const char *envVarName, char **envVarValue)
{
    static miscDYN_BUF envVarValueDynBuf;
    char               *chrPtr;

    if (miscDynBufReset(&envVarValueDynBuf) == FAILURE)
    {
        return FAILURE;
    }
    
    *envVarValue = NULL;

    if ((chrPtr = getenv(envVarName)) == NULL)
    {
        errAdd(miscERR_FILE_ENV_VAR_NOT_DEF, envVarName);
        return FAILURE;
    }

    if (miscDynBufAppendString(&envVarValueDynBuf, chrPtr) == FAILURE)
    {
        return FAILURE;
    }

    envVarValueDynBuf.storedBytes--;   
    *envVarValue = envVarValueDynBuf.dynBuf;

    return SUCCESS;
}

/**
 * Remove the last path or filename in a given path.
 *
 * \param path a null-terminated char array containing the path to be yanked
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscYankLastPath(char *path)
{
    char *chrPtr = NULL;

    if ((chrPtr = strrchr(path, '/')) == NULL)
    {
        /* There is no '/' in the received path */
        return SUCCESS;
    }

    *chrPtr = '\0';

    return SUCCESS;
}


/*___oOo___*/
