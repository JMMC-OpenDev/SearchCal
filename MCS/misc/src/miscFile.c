/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: miscFile.c,v 1.3 2004-06-18 10:56:54 lafrasse Exp $"; 
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
 * \param fullPath full file name.
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
 * This function returns the file extension with any leading directory
 * components removed. For example, miscGetFileName("../data/myFile.fits") will
 * return "fits".
 *
 * \param fullPath full file name.
 * \return the file name.
 */
char *miscGetExtension(char *fullPath)
{
    char *chrPtr, *chrPtr2;

    /* Makes chrPtr points to the last occurence of '.' */
    if ((chrPtr = strrchr(fullPath, '.')) == NULL)
    {
    	/* Exits if no extension found */
        return NULL;
    }
    else
	{
    	/* Check that the 'extension' found is not part of the filepath	   
	     * like e.g.:
    	 *
	     * "/home/ins1b/jknudstr/.fhtHome/DICTIONARY/ALL"
    	 *
	     * Check also the following:
    	 *
	     * "/home/ins1b/jknudstr/.fhtHome"
    	 *
	     * In those cases there is no extension.
    	 */
        /* Makes chrPtr2 points to the last occurence of '/' */
    	chrPtr2 = strrchr(fullPath, '/');
	    if (chrPtr2 > chrPtr)
        {
            /* "/home/ins1b/jknudstr/.fhtHome/DICTIONARY/ALL" */
    	    return NULL;
        }
    	else if (*(chrPtr - 1) == '/')
        {
            /* "/home/ins1b/jknudstr/.fhtHome" */
    	    return NULL;
        }
    	else
        {
	        return (chrPtr + 1);
        }
	}
}

/*___oOo___*/
