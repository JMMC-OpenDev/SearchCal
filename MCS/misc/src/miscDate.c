/*******************************************************************************
 * JMMC project
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2005/02/22 10:08:15  gluck
 * Code review corrections
 *
 * Revision 1.10  2005/02/15 09:40:34  gzins
 * Added CVS log as file modification history
 *
 * Revision 1.9  2005/01/28 18:39:10  gzins
 * Changed FAILURE/SUCCESS to mcsFAILURE/mscSUCCESS
 *
 * gzins     16-Jun-2004  Created
 * lafrasse  17-Jun-2004  Added miscGetLocalTimeStr
 * lafrasse  22-Jul-2004  Added error management
 * lafrasse  23-Jul-2004  Added error management code optimisation
 * lafrasse  02-Aug-2004  Changed includes to isolate miscDate headers from
 *                        misc.h
 *                        Moved mcs.h include to miscDate.h
 *
 ******************************************************************************/

/**
 * \file
 * Definition of miscDate functions.
 */

static char *rcsId="@(#) $Id: miscDate.c,v 1.12 2005-02-22 10:23:35 gluck Exp $"; 
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
#include "err.h"


/* 
 * Local Headers
 */
#include "miscDate.h"
#include "miscPrivate.h"
#include "miscErrors.h"


/**
 * Format the current date and time as YYYY-MM-DDThh:mm:ss.
 *
 * This function generates the string corresponding to the current date,
 * expressed in Coordinated Universal Time (UTC), using the following format
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 * \code 2004-06-16T16:16:48.029 \endcode
 *
 * The number of digits used to represent the the Nth of seconds is given by
 * the \em precision argument.
 *
 * \param utcTime character array where the resulting date is stored
 * \param precision number of digits to be used for the Nth of seconds. The
 * valid range of this argument is 0 to 6.
 * 
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetUtcTimeStr(mcsSTRING32 utcTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get the system time */
    if (gettimeofday(&time, NULL))
    {
        errAdd(miscERR_FUNC_CALL, "gettimeofday");
        return mcsFAILURE;
    }
 
    /* Compute the UTC time from it */
    timeNow = gmtime(&time.tv_sec);

    if (timeNow == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "gmtime");
        return mcsFAILURE;
    }

    /* Format a string from it */
    if (!strftime(utcTime, sizeof(mcsSTRING32), "%Y-%m-%dT%H:%M:%S", timeNow))
    {
        errAdd(miscERR_FUNC_CALL, "strftime");
        return mcsFAILURE;
    }

    /* Add milli-seconds, if requested */
    precision = mcsMIN(precision, 6);
    
    if (precision > 0)
    {
        mcsSTRING32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);

        if (strcpy(tmpBuf, (tmpBuf + 1)) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcpy");
            return mcsFAILURE;
        }

        if (strcat(utcTime, tmpBuf) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcat");
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}


/**
 * Format the current date and time as YYYY-MM-DDThh:mm:ss.
 *
 * This function generates the string corresponding to the current date,
 * expressed in Local Time, using the following format 
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 * \code 2004-06-16T16:16:48.029 \endcode 
 * 
 * The number of digits used to represent the the Nth of seconds is given by
 * the \em precision argument. 
 *
 * \param localTime character array where the resulting date is stored
 * \param precision number of digits to be used for the Nth of seconds. The
 * valid range of this argument is 0 to 6.
 *
 * \n
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetLocalTimeStr(mcsSTRING32 localTime, mcsINT32 precision)
{
    struct timeval  time;
    struct tm       *timeNow;

    /* Get the system time */
    if (gettimeofday(&time, NULL))
    {
        errAdd(miscERR_FUNC_CALL, "gettimeofday");
        return mcsFAILURE;
    }
 
    /* Compute the local time from it */
    timeNow = localtime(&time.tv_sec);

    if (timeNow == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "gmtime");
        return mcsFAILURE;
    }

    /* Compute a string from it */
    if (!strftime(localTime, sizeof(mcsSTRING32), "%Y-%m-%dT%H:%M:%S", timeNow))
    {
        errAdd(miscERR_FUNC_CALL, "strftime");
        return mcsFAILURE;
    }
 
    /* Add milli-seconds, if requested */
    precision = mcsMIN(precision, 6);

    if (precision > 0)
    {
        mcsSTRING32 format, tmpBuf;

        sprintf(format, "%%.%df", precision);
        sprintf(tmpBuf, format, time.tv_usec/1e6);

        if (strcpy(tmpBuf, (tmpBuf + 1)) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcpy");
            return mcsFAILURE;
        }
        
        if (strcat(localTime, tmpBuf) == NULL)
        {
            errAdd(miscERR_FUNC_CALL, "strcat");
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
