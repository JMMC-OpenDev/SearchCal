/*******************************************************************************
 * JMMC project
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.15  2005/01/28 18:39:10  gzins
 * Changed FAILURE/SUCCESS to mcsFAILURE/mscSUCCESS
 *
 * gzins     16-Jun-2004  Created
 * lafrasse  17-Jun-2004  Added miscStrToUpper
 * gzins     23-Jul-2004  Added miscIsSpaceStr
 * lafrasse  23-Jul-2004  Added error management
 * lafrasse  02-Aug-2004  Changed includes to isolate miscFile headers from
 *                        misc.h
 *                        Moved mcs.h include to miscString.h
 * gzins     15-Dec-2004  Added miscTrimString function
 * gzins     16-Dec-2004  Added miscDuplicateString function
 * lafrasse  17-Jan-2005  Added miscSplitString function
 *
 ******************************************************************************/

/**
 * \file
 * Contains all the 'misc' String related functions definitions.
 */

static char *rcsId="@(#) $Id: miscString.c,v 1.16 2005-01-31 12:54:45 gluck Exp $";
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
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscStripQuotes(char *string)
{
    char  *srcPtr;
    char  *dstPtr;

    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return mcsFAILURE;
    }

    /* Worst-case string which becomes:
     *   |   "   kjkdjd kjkjk   kjkj  "  ;   |
     *   |kjkdjd kjkjk   kjkj|
     */

    dstPtr = string;
    /* Find first '"' */
    srcPtr = strchr(string, '\"');

    /* If a quote has been found '"' */
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

    return mcsSUCCESS;
}

/**
 * Trim a string for leading and trailing characters
 *
 * Trim a string for leading and trailing characters according to the 
 * characters given in the  \em trimChars string. The \em trimChars could be
 * e.g. "{} ". In this case the string would be trimmed for all leading
 * "{"'s, "}"'s and spaces.
 *
 * \param string the null-terminated string that shall be trimmed
 * \param trimChars leading and trailing characters to be removed
 *
 * \return always mcsSUCCESS
 */
mcsCOMPL_STAT miscTrimString(char *string, char *trimChars)
{
    char        *chrPtr;
    mcsLOGICAL  run;

    /* If pointer is not null */ 
    if (*string != '\0')
    {
        /* Remove leading trim characters; i.e. look for the first character
         * which is not a character to be trimmed */
        run = mcsTRUE;
        chrPtr = string;
        do
        {
            if (strchr(trimChars, *chrPtr) != NULL)
            {
                chrPtr++;
            }
            else
            {
                run = mcsFALSE;
            }
        } while ((*chrPtr != '\0') && run);

        /* If leading trim characters have been found */ 
        if (string != chrPtr)
        {
            /* Copy string form the first 'good' character */
            strcpy(string, chrPtr);
        }
        /* End if */

        /* Remove trailing trim characters */
        if (*chrPtr != '\0')
        {
            /* Got to the last characters and look for the first character
             * which is not a character to be trimmed  */
            chrPtr = (string + (strlen(string) - 1));
            run = mcsTRUE;
            do
            {
                if (strchr(trimChars, *chrPtr) != NULL)
                {
                    *chrPtr = '\0';
                    chrPtr--;
                }
                else
                    run = mcsFALSE;
            }
            while ((*chrPtr != '\0') && run);
        }
    }
    return mcsSUCCESS;
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
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscStrToUpper(char *string)
{
    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return mcsFAILURE;
    }

    while (*string != '\0')
    {
        *string = toupper(*string);
        string++;
    }

    return mcsSUCCESS;
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
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
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
        return mcsFAILURE;
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

    return mcsSUCCESS;
}

/**
 * Duplicate a string.
 * 
 * The miscDuplicateString function returns a pointer to a new string which is a
 * duplicate of the given string. Memory for the new string is obtained with
 * malloc(3), and can be freed with free(3)
 *
 * \param string the null-terminated string to be duplicated.
 * 
 * \return a pointer to the duplicated string, or NULL if insufficient memory
 * was available
 */
char *miscDuplicateString (const char *string)
{
    char * newString;
   
    /* Create new string */
    newString = malloc(strlen(string) + 1);
    if (newString == NULL)
    {
        errAdd(miscERR_ALLOC);
        return NULL;
    }

    /* Duplicate string */
    strcpy(newString, string);

    return newString;
}

/**
 * Split a string on a given delimiter.
 *
 * Copy each sub-string in the already allocated string array passed in
 * parameter. The number of found sub-string is returned by the 'subStringNb'
 * parameter.
 * 
 * \param string the null-terminated string to be parsed.
 * \param delimiter the character on which the sub-strings should be found.
 * \param subStrings the allocated array used to return the null-terminated
 * sub-strings.
 * \param maxSubStringNb the maximum number of sub-strings the sub-string array
 * can hold.
 * \param subStringNb the number of found sub-strings.
 * 
 * \return an MCS completion status code (mcsSUCCESS or mcsFAILURE)
 */
mcsCOMPL_STAT miscSplitString    (const char         *string,
                                  const char          delimiter,
                                        mcsSTRING256  subStrings[],
                                  const mcsUINT32     maxSubStringNb,
                                        mcsUINT32    *subStringNb)
{
    /* If any of the received parameters is unvalid */
    if (string == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "string");
        return mcsFAILURE;
    }
    if (subStrings == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "subStrings");
        return mcsFAILURE;
    }
    if (maxSubStringNb <= 0)
    {
        errAdd(miscERR_NULL_PARAM, "maxSubStringNb");
        return mcsFAILURE;
    }
    if (subStringNb == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "subStringNb");
        return mcsFAILURE;
    }

    char*     floatingPtr        = (char*)string;
    char*     subString          = NULL;
    mcsUINT32 length             = 0;
    mcsUINT32 i                  = 0;
    mcsUINT32 maxSubStringLength = sizeof(subStrings[i]) - 1;
   
    /* While some occurences of the delimiter are found inside the string ... */
    while (((floatingPtr - string) < strlen(string)) && (floatingPtr != NULL))
    {
        /* Get the next deilmiter position */
        subString = strchr(floatingPtr, delimiter);

        /* If the sub-string array is not full yet... */
        if (i < maxSubStringNb)
        {
            /* Compute the sub-string length between its real length, and its
             * maximun possible length (defined by the sub-string array type)
             */
            length = mcsMIN((subString - floatingPtr), maxSubStringLength);

            /* Copy the sub-string in the sub-string array */
            strncpy(subStrings[i], floatingPtr, length);
            /* Add end of string explicitly */
            subStrings[i][length] = '\0';
        }
        else
        {
            errAdd(miscERR_STRING_MAX_SUBSTRING_NB_OVERFLOW, maxSubStringNb);
            return mcsFAILURE;
        }

        i++;
        floatingPtr = subString + 1;
    }

    /* Return the number of sub-string found */
    *subStringNb = i;
    return mcsSUCCESS;
}


/*___oOo___*/
