/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of miscDate functions.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscDate.c,v 1.18 2006-01-10 14:40:39 mella Exp $"; 


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

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


/*
 * Enumeration type definition
 */

/* Type time */
typedef enum
{
    miscUTC_TIME,       /* UTC time */
    miscLOCAL_TIME      /* Local time */
} miscTIME_TYPE;


/* 
 * Local functions declaration 
 */
static mcsCOMPL_STAT miscGetTimeStr(const miscTIME_TYPE  timeType, 
                                          mcsUINT32      precision,
                                          mcsSTRING32    computedTime);


/* 
 * Local functions definition
 */

/**
 * Give back the current @em UTC date and time as a null-terminated string.
 *
 * This function generates the string corresponding to the current date,
 * expressed either in Coordinated Universal Time (UTC) or in Local Time,
 * depending on time type and using the following format
 * YYYY-MM-DDThh:mm:ss[.ssssss], as shown in the following example :
 * @code 2004-06-16T16:16:48.029 @endcode
 *
 * The number of digits used to represent the the Nth of seconds is given by
 * the @em precision argument.
 *
 * @param timeType time type (UTC or local) which corresponds to miscUTC_TIME or
 * miscLOCAL_TIME values respectively (see miscTIME_TYPE)
 * @param precision number of digits to be used for the Nth of seconds, between
 * 0 and 6.
 * @param computedTime null-terminated string where the resulting date is
 * stored
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
static mcsCOMPL_STAT miscGetTimeStr(const miscTIME_TYPE  timeType, 
                                          mcsUINT32      precision,
                                          mcsSTRING32    computedTime)
{
    struct timeval  time;
    struct tm      *timeNow;
    mcsSTRING32     timeComputingError, format, tmpBuf;

    /* Get the system time */
    if (gettimeofday(&time, NULL))
    {
        errAdd(miscERR_FUNC_CALL, "gettimeofday", strerror(errno));
        return mcsFAILURE;
    }

    /* Compute time in seconds from it, depending on time type */
    if (timeType == miscUTC_TIME)
    {
        /* utc time */
        timeNow = gmtime(&time.tv_sec);
        sprintf(timeComputingError, "%s", "gmtime");
    }
    else
    {
        /* local time */
        timeNow = localtime(&time.tv_sec);
        sprintf(timeComputingError, "%s", "localtime");
    }

    if (timeNow == NULL)
    {
        errAdd(miscERR_FUNC_CALL, timeComputingError, strerror(errno));
        return mcsFAILURE;
    }

    /* Create a string from it */
    if (strftime(computedTime, sizeof(mcsSTRING32), "%Y-%m-%dT%H:%M:%S", 
                  timeNow) == 0)
    {
        errAdd(miscERR_FUNC_CALL, "strftime", strerror(errno));
        return mcsFAILURE;
    }

    /* Get microseconds in 0.[xxxxxx] format if requested */
    sprintf(format, "%%.%uf", mcsMIN(precision, 6u));
    sprintf(tmpBuf, format, time.tv_usec/1e6);

    /*
     * Concatenate microseconds to computed time, skipping the first '0' =>
     * index 1 for tmpBuf
     */
    if (strcat(computedTime, &tmpBuf[1]) == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "strcat", strerror(errno));
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*
 * Public functions definition
 */

/**
 * Give back the current @em UTC date and time as a null-terminated string.
 *
 * This function generates a string corresponding to the current date expressed
 * in Coordinated Universal Time (UTC) using the "YYYY-MM-DDThh:mm:ss[.ssssss]"
 * format, as shown in the following example :
 * @code 2004-06-16T16:16:48.029 @endcode
 *
 * The number of digits used to represent the Nth of seconds is given by the @em
 * precision argument.
 *
 * @param precision number of digits to be used for the Nth of seconds, between
 * 0 and 6.
 * @param utcTime null-terminated string where the resulting date is stored.
 * 
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetUtcTimeStr(const mcsUINT32 precision, mcsSTRING32 utcTime)
{
    if (miscGetTimeStr(miscUTC_TIME, precision, utcTime) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/**
 * Give back the current @em local date and time as a null-terminated string.
 *
 * This function generates a string corresponding to the current date expressed
 * in Local Time using the "YYYY-MM-DDThh:mm:ss[.ssssss]" format, as shown in
 * the following example :
 * @code 2004-06-16T16:16:48.029 @endcode 
 * 
 * The number of digits used to represent the Nth of seconds is given by the @em 
 * precision argument. 
 *
 * @param precision number of digits to be used for the Nth of seconds, between
 * 0 and 6.
 * @param localTime null-terminated string where the resulting date is stored.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetLocalTimeStr(const mcsUINT32    precision,
                                        mcsSTRING32  localTime)
{
    if (miscGetTimeStr(miscLOCAL_TIME, precision, localTime) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
