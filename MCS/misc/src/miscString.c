/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  Created
* lafrasse  17-Jun-2004  Added miscStrToUpper
* gzins     23-Jul-2004  Added miscIsSpaceStr
* lafrasse  23-Jul-2004  Added error management
* lafrasse  02-Aug-2004  Changed includes to isolate miscFile headers from
*                        misc.h
*                        Moved mcs.h include to miscString.h
*
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Contains all the 'misc' String related functions definitions.
 */

static char *rcsId="@(#) $Id: miscString.c,v 1.10 2004-09-27 07:48:13 scetre Exp $";
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


/*
 * MCS Headers
 */
#include "err.h"


/*
 * Local Headers
 */
#include "miscString.h"
#include "miscPrivate.h"
#include "miscErrors.h"


/**
 * Strip quotes enclosing a string.
 *
 * Strings are sometimes enclosed in quotes. This function strips these off
 * using the same character buffer for storing the processed string. The
 * string must be NULL terminated.
 *
 * If the string is not contained in quotes, the function simply returns
 * without changing the string.
 *
 * \param string the null-terminated string that shall be stripped
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscStripQuotes(char *string)
{
    char  *srcPtr;
    char  *dstPtr;

    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return FAILURE;
    }

    /* Worst-case string which becomes:
     *   |   "   kjkdjd kjkjk   kjkj  "  ;   |
     *   |kjkdjd kjkjk   kjkj|
     */

    dstPtr = string;
    /*= Find first '"' */
    srcPtr = strchr(string, '\"');

    /*= If a quote has been found '"' */
    if (srcPtr != NULL)
    {
        /* Skip possible blanks after the quote */
        srcPtr++;
        while (*srcPtr == ' ')
        {
            srcPtr++;
        }

        /* Copy until quote or NULL terminator */
        while ((*srcPtr != '\0') && (*srcPtr != '\"')) /* " */
        {
            *dstPtr = *srcPtr;
            dstPtr++;
            srcPtr++;
        }

        /* If the string only contains blanks or is of length 0, dstPtr still
         * points to the beginning of the string
         */

        /* Yank possible trailing blanks */
        if (dstPtr != string)
        {
            dstPtr--;
            while (*dstPtr == ' ')
                dstPtr--;
            dstPtr++;
        }
        *dstPtr = '\0';
    }

    return SUCCESS;
}

/**
 * Convert a string to upper case.
 *
 * Strings are sometimes composed of mixed caracaters case. This function
 * cleans this by upper-casing all the caracters, using the same character
 * buffer for storing the processed string. The string must be NULL terminated.
 *
 * \param string the null-terminated string that shall be upper-cased
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscStrToUpper(char *string)
{
    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return FAILURE;
    }

    while (*string != '\0')
    {
        *string = toupper(*string);
        string++;
    }

    return SUCCESS;
}

/**
 * Checks for string only containing white-space characters.
 * 
 * It returns true (i.e. mcsTRUE) if string only contains white-space (i.e.
 * blank), and false (i.e. mcsFALSE) otherwise.
 *
 * \warning string must \em NOT be a null pointer.\n\n
 *
 * \param string the null-terminated string that shall be checked.
 *
 * \return mcsTRUE if it is a white-space string, mcsFALSE otherwise.
 */
mcsLOGICAL miscIsSpaceStr (char *string)
{
    while (*string != '\0')
    {
		if (!isspace(*string))
        {
			return mcsFALSE;
        }
        string++;
    }
	return mcsTRUE;
}

/**
 * Replace a character occurence by another one in a string.
 * 
 * \warning string \em must be a NULL terminated char array pointer.\n\n
 *
 * \param string the null-terminated string that shall be modified.
 * \param originalChar the character to be replaced.
 * \param newChar the replacing character. 
 * 
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscReplaceChrByChr (char *string,
                                   char originalChar,
                                   char newChar)
{
    int i=0;
   
    /* Check string parameter validity */
    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return FAILURE;
    }

    /* For each charracter of the string */ 
    while (string[i] !=  '\0')
    {
        /* Check if the current character has to be replaced */
        if (string[i] == originalChar)
        {
            /* if ok, replace it */
            string[i] = newChar;
        }
        i++;        
    }

    return SUCCESS;
}

/*___oOo___*/
