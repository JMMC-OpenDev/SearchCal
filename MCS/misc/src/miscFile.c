/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
* lafrasse  17-Jun-2004  added miscGetExtension
* lafrasse  18-Jun-2004  debugged miscGetExtension
*                        added miscYankExtension
*
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: miscFile.c,v 1.5 2004-06-22 07:58:33 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/* 
 * MCS Headers
 */
#include "mcs.h"

/* 
 * Local Headers
 */
#include "misc.h"

/**
 * Returns the file name.
 * 
 * This function returns the file name with any leading directory components
 * removed. For example, miscGetFileName("../data/myFile.fits") will return
 * "myFile.fits".
 *
 * \param fullPath the full path.
 * \return the file name.
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
    /* End if */

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
    strcpy(buffer, fullPath);

    /* Establish string and get the first token: */
    token = strtok( buffer, "/" );
    /* While there are tokens in "string" */       
    while( token != NULL )
    {
        /* Get next token: */
        fileName = token;

        token = strtok( NULL, "/" );   
    }
    /* End while */

    return (char*)fileName;
}

/**
 * Returns the file extension.
 * 
 * This function returns the extention of the given file name, i.e. the
 * characters after the last dot in the file name.
 * miscGetExtension("../data/myFile.fits") will return "fits".
 *
 * /warning : NULL is returned when there is no dot in the file name or the
 * last dot is the first character of the file name,e.g.:
 *              "/data/.dt"
 *              "/data/.dt/cache"
 * \param fileName the file name.
 * \return the file extension.
 */
char *miscGetExtension(char *fileName)
{
    char *chrPtr, *chrPtr2;

    /* Makes chrPtr points to the last occurence of '.' */
    if ((chrPtr = strrchr(fileName, '.')) == NULL)
    {
    	/* Exits if no extension found */
        return NULL;
    }
    else
	{
    	/* Check that the 'extension' found is not part of the filepath	   
	     * like e.g.:
    	 *
	     * "/data/.dt/cache"
    	 *
	     * Check also the following:
    	 *
	     * "/data/.dt"
    	 *
	     * In those cases there is no extension.
    	 */
        /* Makes chrPtr2 points to the last occurence of '/' */
    	chrPtr2 = strrchr(fileName, '/');
	    if (chrPtr2 > chrPtr)
        {
            /* "/data/.dt/cache" */
    	    return NULL;
        }
    	else if (*(chrPtr - 1) == '/')
        {
            /* "/data/.dt" */
    	    return NULL;
        }
    	else
        {
	        return (chrPtr + 1);
        }
	}
}

/**
 * Returns the file name without the file extension.
 * 
 * This function cats down the given file extension off from the file name
 * using the same original buffer.
 * Note that when filename does not end with the given extention the function
 * has no effect.
 * The extention can be given with or without the dots, e.g.:  "fits", or
 * ".fits".
 * If \em extension is a null pointer, this function cats down the extention,
 * found using miscGetExtension(), of the given 'fileName' if it has
 * any extention.
 *
 * For example, miscYankExtension("../data/myFile.fits") will give back 
 * "../data/myFile".
 *
 * \warning When extension is specified, the function looks for the FIRST
 * occurance of the \em extention in the \em fileName ! Therefore if the given
 * file name is :'file.fitsname.fits' and the extention is 'fits' the
 * resulting file name is :'file'.
 *
 * \param fileName the file name.
 * \param extension the file name.
 * \return file name without extension 
 */
void miscYankExtension(char *fileName, char *extension)
{    
    int      pos;
    char     *extPtr;

    /* return if the file name is null */
    if (fileName == NULL)
    {
        return;
    }

    /* if extension is specified */
    if (extension != NULL)
    {
        mcsSTRING32 extLoc;
        if (*extension != '.')
            sprintf(extLoc, ".%s", extension);
        else
            sprintf(extLoc, "%s", extension);
        if ((extPtr = strstr(fileName, extLoc)) != NULL)
            *extPtr = '\0';
    }
    else
    {
        /* get the extention */
        if ((extPtr = miscGetExtension(fileName)) == NULL)
        {
            return;
        }
        else
        {
            /* find the position of the extension */
            pos = strlen(fileName)-strlen(extPtr)-1;

            /* cut the string there */
            fileName[pos] = '\0';
        }
    }
}

/*___oOo___*/
