/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: logPrivate.c,v 1.2 2004-08-10 13:29:10 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  05-Aug-2004  Created
* lafrasse  10-Aug-2004  Moved logGetTimeStamp back in log.c
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: logPrivate.c,v 1.2 2004-08-10 13:29:10 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>

#include <sys/utsname.h>

#include <stdarg.h>
#include <stdlib.h>


/*
 * MCS Headers 
 */
#include "mcs.h"


/* 
 * Local Headers
 */
#include "logPrivate.h"


/*
 * Local Functions
 */

/**
 * Give back the local network host name.
 *
 * In case the NULL-terminated host name does not fit, no  error is returned
 * but the host name is truncated. It is unspecified whether the truncated
 * host name will be NULL-terminated.
 *
 * \warning As is, this function uses 'uname()' SysV call, so it is not portable
 * to BSD-style systems that require 'gethostname()' system call.\n\n
 *
 * \param hostName allocated character array where the resulting date is stored
 * \param length allocated character array length
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT logGetHostName(char *hostName, mcsUINT32 length)
{
    struct utsname systemInfo;

    /* Test 'hostName' parameter validity */
    if (hostName == NULL)
    {
        return FAILURE;
    }

    /* Test 'length' parameter validity */
    if (length == 0)
    {
        return FAILURE;
    }

    /* Try to get the host name from the system */
    if (uname(&systemInfo) != 0)
    {
        return FAILURE;
    }

    if (strncpy((char *)hostName, systemInfo.nodename, length) == NULL)
    {
        return FAILURE;
    }

    return SUCCESS;
}


/**
 * Print a message on stderr.
 *
 * \param format format of given message
 */
void logDisplayMessage(const char *format, ...)
{
    va_list argPtr;

    mcsBYTES32 utcTime;

    /* Display the current UTC time */
    logGetTimeStamp(utcTime);
    fprintf(stderr, utcTime);
    fprintf(stderr, " : ");

    /* Display the current process name */
    fprintf(stderr, mcsGetProcName());
    fprintf(stderr, " ");

    /* Display the variable parameters */
    va_start(argPtr, format);
    vfprintf(stderr, format, argPtr);
    va_end(argPtr);

    fprintf(stderr, ".\n");
    fflush(stderr);
}


/**
 * Print a message on stderr, and exit with EXIT_FAILURE return code.
 *
 * \param format format of given message
 */
void logDisplayError(const char *format, ...)
{
    va_list argPtr;

    /* Display the variable parameters */
    va_start(argPtr, format);
    logDisplayMessage(format, argPtr);
    va_end(argPtr);

    fprintf(stderr, "Aborted.\n");
    exit(EXIT_FAILURE);
}


/*___oOo___*/
