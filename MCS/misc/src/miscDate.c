/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  Created
* lafrasse  17-Jun-2004  Added miscGetLocalTimeStr
* lafrasse  22-Jul-2004  Added error management
* lafrasse  23-Jul-2004  Added error management code optimisation
*
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Contains all the 'misc' Date and Time related functions definitions.
 *
 * \sa To see all the other 'misc' module functions declarations, see misc.h
 */

static char *rcsId="@(#) $Id: miscDate.c,v 1.5 2004-07-23 14:29:59 lafrasse Exp $"; 
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
#include "err.h"


/* 
 * Local Headers
 */
#include "misc.h"
#include "miscPrivate.h"
#include "miscErrors.h"


/**
 * Format the current date and time as YYYY-MM-DDThh:mm:ss.
 *
 * This function generates the string corresponding to the current date,
 * expressed in Coordinated Universal Time (UTC), using the following format
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 *    
 *     2004-06-16T16:16:48.029
 * 
 * The number of digits used to represent the the Nth of seconds is given by
 * the \em precision argument. The valid range of this argument is 0 to 6.
 *
 * \param utcTime character array where the resulting date is stored
 * \param precision number of digits to be used for the Nth of seconds
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscGetUtcTimeStr(mcsBYTES32 utcTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get the system time */
    if (gettimeofday(&time, NULL))
    {
        errAdd(miscERR_FUNC_CALL, "gettimeofday");
        return FAILURE;
    }
 
    /* Compute the UTC time from it */
    if ((timeNow = gmtime(&time.tv_sec)) == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "gmtime");
        return FAILURE;
    }

    /* Compute a string from it */
    if (!strftime(utcTime, sizeof(mcsBYTES32), "%Y-%m-%dT%H:%M:%S", timeNow))
    {
        errAdd(miscERR_FUNC_CALL, "strftime");
        return FAILURE;
    }

    /* Add milli-seconds, if requested */
    precision=mcsMIN(precision, 6);

    if (precision > 0)
    {
        mcsBYTES32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);

        if (strcpy(tmpBuf, (tmpBuf + 1)) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcpy");
            return FAILURE;
        }

        if (strcat(utcTime, tmpBuf) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcat");
            return FAILURE;
        }
    }

    return SUCCESS;
}

/**
 * Format the current date and time as YYYY-MM-DDThh:mm:ss.
 *
 * This function generates the string corresponding to the current date,
 * expressed in Local Time, using the following format 
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 *    
 *     2004-06-16T16:16:48.029
 * 
 * The number of digits used to represent the the Nth of seconds is given by
 * the \em precision argument. The valid range of this argument is 0 to 6.
 *
 * \param localTime character array where the resulting date is stored
 * \param precision number of digits to be used for the Nth of seconds
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get the system time */
    if (gettimeofday(&time, NULL))
    {
        errAdd(miscERR_FUNC_CALL, "gettimeofday");
        return FAILURE;
    }
 
    /* Compute the local time from it */
    if ((timeNow = localtime(&time.tv_sec)) == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "gmtime");
        return FAILURE;
    }

    /* Compute a string from it */
    if (!strftime(localTime, sizeof(mcsBYTES32), "%Y-%m-%dT%H:%M:%S", timeNow))
    {
        errAdd(miscERR_FUNC_CALL, "strftime");
        return FAILURE;
    }
 
    /* Add milli-seconds, if requested */
    precision=mcsMIN(precision, 6);

    if (precision > 0)
    {
        mcsBYTES32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);

        if (strcpy(tmpBuf, (tmpBuf + 1)) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcpy");
            return FAILURE;
        }
        
        if (strcat(localTime, tmpBuf) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcat");
            return FAILURE;
        }
    }

    return SUCCESS;
}

/*___oOo___*/
