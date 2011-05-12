/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Declaration of miscString functions.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscString.c,v 1.25 2008-05-16 09:24:59 lafrasse Exp $";


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


/*
 * Public functions definition
 */

/**
 * Strip quotes and spaces (if any) enclosing a given null-terminated string.
 *
 * Strings are sometimes enclosed in quotes. This function strips them off (if
 * any) using the same given character buffer to give back the resulting string.
 *
 * @param str the null-terminated string that shall be stripped.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscStripQuotes(char *str)
{
    /* Verify paramater vaildity */
    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }

    /* Remove any leading or trailing spaces and quotes */
    return miscTrimString(str, "\" ");
}


/**
 * Trim a null-terminated string for the given leading and trailing characters.
 *
 * For example, if @em trimChars contains "{} ", the given string would be given 
 * back trimmed for all "{"'s, "}"'s and spaces, using the same given character
 * buffer to give back the resulting string.
 *
 * @param str the null-terminated string that shall be trimmed.
 * @param trimChars leading and trailing characters to be removed.
 *
 * @return always mcsSUCCESS.
 */
mcsCOMPL_STAT miscTrimString(char *str, const char *trimChars)
{
    char        *currentChrPtr;
    mcsLOGICAL   run;

    /* If pointer is not null */ 
    if (*str != '\0')
    {
        /*
         * Remove leading trim characters; i.e. look for the first character
         * which is not a character to be trimmed.
         */
        run = mcsTRUE;
        currentChrPtr = str;
        do
        {
            /* If the current character must be trimed... */
            if (strchr(trimChars, *currentChrPtr) != NULL)
            {
                /* Skip it */
                currentChrPtr++;
            }
            else
            {
                /* Exit from the loop */
                run = mcsFALSE;
            }
        }
        while ((*currentChrPtr != '\0') && run);

        /* If any leading trim characters have been found */ 
        if (str != currentChrPtr)
        {
            /* Copy str from the first 'good' character */
            strcpy(str, currentChrPtr);
        }

        /* Remove any trailing trim characters */
        if (*currentChrPtr != '\0')
        {
            /*
             * Got to the last characters and look for the first character
             * which is not a character to be trimmed
             */
            currentChrPtr = str + (strlen(str) - 1);
            run = mcsTRUE;
            do
            {
                /* If the current character must be trimed... */
                if (strchr(trimChars, *currentChrPtr) != NULL)
                {
                    /* Blank the string at its position */
                    *currentChrPtr = '\0';
                    currentChrPtr--;
                }
                else
                {
                    /* Exit from the loop */
                    run = mcsFALSE;
                }
            }
            while ((*currentChrPtr != '\0') && run);
        }
    }

    return mcsSUCCESS;
}


/**
 * Convert a null-terminated string to upper case.
 *
 * Strings are sometimes composed of mixed characters case. This function
 * cleans this by upper-casing all those characters, using the same given
 * character buffer to give back the resulting string.
 *
 * @param str the null-terminated string that shall be upper-cased.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscStrToUpper(char *str)
{
    /* Verify parameter validity */
    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }

    /* For each character in str... */
    while (*str != '\0')
    {
        /* Convert it to uppercase */
        *str = toupper(*str);
        str++;
    }

    return mcsSUCCESS;
}


/**
 * Return whether the given null-terminated string contains only white-space
 * characters or not.
 * 
 * It returns true if the given null-terminated string is empty or contains only
 * white-space (i.e. blank), false otherwise.
 *
 * @warning str must @em NOT be a null pointer.\n\n
 *
 * @param str the null-terminated string that shall be checked.
 *
 * @return mcsTRUE if it is a white-space string, mcsFALSE otherwise.
 */
mcsLOGICAL miscIsSpaceStr(const char *str)
{
    /* For each character in str... */
    while (*str != '\0')
    {
        /* If the current character is not a blank... */
        if (isspace(*str) == 0)
        {
            /* Return false */
            return mcsFALSE;
        }

        str++;
    }

    return mcsTRUE;
}


/**
 * Return whether the given line is a comment line or not.
 * 
 * It returns true if the line begins with the given comment pattern (with or
 * without any leading spaces or tabs), false otherwise.
 *
 * @warning line @em MUST be '\\n' or 'null' terminated.\n\n
 *
 * @param line the line that shall be checked.
 * @param commentPatternStr the null-terminated string characterizing a comment
 * line.
 *
 * @return mcsTRUE if it is a comment line, mcsFALSE otherwise.
 */
mcsLOGICAL miscIsCommentLine(const char       *line,
                             const mcsSTRING4  commentPatternStr)
{
    mcsUINT32 commentPatternLength;

    /* Skip any leading spaces or tabs */
    while ((*line == ' ') || (*line == '\t'))
    {
        line++;
    }

    /* If it is a comment line */
    commentPatternLength = strlen(commentPatternStr);
    if ((strlen(commentPatternStr) != 0) &&
        (strncmp(line, commentPatternStr, commentPatternLength) == 0))
    {
        /* Return true */
        return mcsTRUE;
    }

    return mcsFALSE;
}


/**
 * Replace a character occurences by another one in a null-terminated string.
 * 
 * @param str the null-terminated string that shall be modified.
 * @param originalChar the character to be replaced.
 * @param newChar the replacing character. 
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscReplaceChrByChr(      char *str,
                                  const char  originalChar,
                                  const char  newChar)
{
    /* Check str parameter validity */
    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }

    /* For each character of str... */ 
    while (*str !=  '\0')
    {
        /* If the current character has to be replaced... */
        if (*str == originalChar)
        {
            /* Replace it */
            *str = newChar;
        }

        str++;
    }

    return mcsSUCCESS;
}


/**
 * Remove the first or all occurences of a given character in a null-terminated
 * string.
 * 
 * @param str the null-terminated string that shall be modified.
 * @param searchedChar the character to be removed.
 * @param allFlag the flag specifying whether all the occurences of the given
 * character (if set to mcsTRUE), or only the first found (if set to mcsFALSE),
 * should be removed. 
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscDeleteChr(      char       *str,
                            const char        searchedChar,
                            const mcsLOGICAL  allFlag)
{
    /* Check str parameter validity */
    mcsINT32 src, dest;
    mcsLOGICAL deleteChr;

    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }

    /* For each character of str... */
    deleteChr = mcsTRUE;
    for (src = 0, dest = 0; str[src] != '\0'; src++)
    {
        /*
         * If it is not the searched character, or if the first occurence of
         * the searched character has been already deleted and the other ones
         * should not be removed...
         */
        if ((str[src] != searchedChar) || (deleteChr == mcsFALSE))
        {
            /* Copy back the character into str */
            str[dest] = str[src];
            dest++;
        }
        else
        {
            /*
             * Skip character, and test whether next searched character has to
             * be removed or not.
             */
            if (allFlag == mcsFALSE)
            {
                deleteChr = mcsFALSE;
            }
        }
    }

    /* Added end-of-string character */
    str[dest] = '\0';

    return mcsSUCCESS;
}


/**
 * Return a copy of a given null-terminated string.
 * 
 * The memory for the new string is obtained with malloc(3), and thus can be
 * freed with free(3).
 *
 * @param str the null-terminated string to be duplicated.
 * 
 * @return a pointer to the duplicated string, or NULL if there is no sufficient
 * memory available.
 */
char *miscDuplicateString(const char *str)
{
    char *newStr;
   
    /* Create a new empty string */
    newStr = malloc(strlen(str) + 1);
    if (newStr == NULL)
    {
        errAdd(miscERR_ALLOC);
        return NULL;
    }

    /* Copy str content in the new string */
    strcpy(newStr, str);

    return newStr;
}


/**
 * Split a null-terminated string on a given delimiter.
 *
 * Copy each sub-string in the already allocated string array passed in
 * parameter. The number of found sub-string is given back by the 'subStringNb'
 * parameter.
 *
 * @warning If any sub-string is longer than the available length of each
 * sub-string array cell (i.e 255 characters), its content will be truncated to
 * fit inside.\n\n
 * 
 * @param str the null-terminated string to be parsed.
 * @param delimiter the character by which the sub-strings should be delimited.
 * @param subStrArray the allocated array used to return the null-terminated
 * sub-strings.
 * @param maxSubStrNb the maximum number of sub-strings the sub-string array
 * can hold.
 * @param subStrNb the number of found sub-strings.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscSplitString(const char         *str,
                              const char          delimiter,
                                    mcsSTRING256  subStrArray[],
                              const mcsUINT32     maxSubStrNb,
                                    mcsUINT32    *subStrNb)
{
    char*     nextDelimPtr;
    char*     subStrPtr;
    mcsUINT32 subStrLength;
    mcsUINT32 foundSubStrNb;
    mcsUINT32 maxSubStrLength;
   
    /* Verify each parameter validity */
    if (str == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "str");
        return mcsFAILURE;
    }
    if (subStrArray == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "subStrArray");
        return mcsFAILURE;
    }
    if (maxSubStrNb <= 0)
    {
        errAdd(miscERR_NULL_PARAM, "maxSubStrNb");
        return mcsFAILURE;
    }
    if (subStrNb == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "subStrNb");
        return mcsFAILURE;
    }

    nextDelimPtr    = NULL;
    subStrPtr       = (char*)str;
    subStrLength    = 0;
    foundSubStrNb   = 0;
    maxSubStrLength = sizeof(*subStrArray) - 1;
   
    /* While there are some delimiter occurences left in str... */
    do
    {
        /* Get the next delimiter position (if any) */
        nextDelimPtr = strchr(subStrPtr, delimiter);

        /* If the sub-string array is not full yet... */
        if (foundSubStrNb < maxSubStrNb)
        {
            /* Compute the sub-string length */
            subStrLength = nextDelimPtr - subStrPtr;
            /*
             * The sub-string length should not exceed the maximun possible
             * length (defined by the sub-string array type)
             */
            subStrLength = mcsMIN(subStrLength, maxSubStrLength);
            /*
             * The sub-string length should not be below 0 (otherwise
             * strncpy() raise a SIGSEV under 64bit platforms)
             */
            subStrLength = mcsMAX(subStrLength, 0);

            /* Copy the sub-string in the sub-string array */
            strncpy(subStrArray[foundSubStrNb], subStrPtr, subStrLength);

            /* Added end-of-string character */
            subStrArray[foundSubStrNb][subStrLength] = '\0';
        }
        else
        {
            errAdd(miscERR_STRING_MAX_SUBSTRING_NB_OVERFLOW, maxSubStrNb);
            return mcsFAILURE;
        }

        foundSubStrNb++;
        subStrPtr = nextDelimPtr + 1;
    }
    while (nextDelimPtr != NULL);

    /* Return the number of sub-strings found */
    *subStrNb = foundSubStrNb;

    return mcsSUCCESS;
}


/*___oOo___*/
