/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: logPrivate.c,v 1.3 2004-11-10 22:09:31 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  05-Aug-2004  Created
* lafrasse  10-Aug-2004  Moved logGetTimeStamp back in log.c
* gzins     10-Nov-2004  Renamed logDisplayMessage to logPrintErrMessage
*                        Removed logDisplayError
*
*******************************************************************************/

static char *rcsId="@(#) $Id: logPrivate.c,v 1.3 2004-11-10 22:09:31 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>

#include <time.h>
#include <errno.h>
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

/**
 * Give back the local network host name.
 *
 * If host name is longer than the given buffer, an error message is print on
 * stderr and FAILURE code is returned.
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
        logPrintErrMessage("logGetHostName() - NULL parameter");
        return FAILURE;
    }

    /* Test 'length' parameter validity */
    if (length == 0)
    {
        logPrintErrMessage("logGetHostName() - invalid 'length' parameter");
        return FAILURE;
    }

    /* Get the host name from the system */
    if (uname(&systemInfo) != 0)
    {
        logPrintErrMessage("uname() failed - %s", strerror(errno));
        return FAILURE;
    }

    /* Test if buffer can contain host name */
    if (strlen(systemInfo.nodename) >= length)
    {
        logPrintErrMessage
            ("logGetHostName() - given buffer too short to store host name '%s'",
             systemInfo.nodename);
        return FAILURE;
    }

    /* Copy the host name */
    strcpy((char *)hostName, systemInfo.nodename);

    return SUCCESS;
}

/**
 * Print a message on stderr.
 *
 * \param format format of given message
 */
void logPrintErrMessage(const char *format, ...)
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

/*___oOo___*/
