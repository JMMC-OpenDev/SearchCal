/*******************************************************************************
* JMMC project
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* gzins     16-Jun-2004  created
*
*-----------------------------------------------------------------------------*/

static char *rcsId="@(#) $Id: miscDate.c,v 1.2 2004-06-17 11:55:58 lafrasse Exp $"; 
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
 */
void miscGetUtcTimeStr(mcsBYTES32 utcTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get UTC time */
    gettimeofday(&time, NULL);
 
    /* Format the date */
    timeNow = gmtime(&time.tv_sec);
    strftime(utcTime, sizeof(mcsBYTES32), "%Y-%m-%dT%H:%M:%S", timeNow);
 
    /* Add milli-seconds, if requested */
    precision=mcsMIN(precision, 6);
    if (precision > 0)
    {
        mcsBYTES32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);
        strcpy(tmpBuf, (tmpBuf + 1));
        strcat(utcTime, tmpBuf);
    }
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
 */
void miscGetLocalTimeStr(mcsBYTES32 localTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get local time */
    gettimeofday(&time, NULL);
 
    /* Format the date */
    timeNow = localtime(&time.tv_sec);
    strftime(localTime, sizeof(mcsBYTES32), "%Y-%m-%dT%H:%M:%S", timeNow);
 
    /* Add milli-seconds, if requested */
    precision=mcsMIN(precision, 6);
    if (precision > 0)
    {
        mcsBYTES32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);
        strcpy(tmpBuf, (tmpBuf + 1));
        strcat(localTime, tmpBuf);
    }
}

/*___oOo___*/
