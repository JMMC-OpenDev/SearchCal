/*******************************************************************************
* JMMC project
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  03-Aug-2004  Created
*
*
*-----------------------------------------------------------------------------*/

/**
 * \file
 * Contains all the 'misc' Network related functions definitions.
 */

static char *rcsId="@(#) $Id: miscNetwork.c,v 1.2 2005-01-19 10:25:38 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <sys/utsname.h>
#include <string.h>


/* 
 * MCS Headers
 */
#include "err.h"


/* 
 * Local Headers
 */
#include "miscNetwork.h"
#include "miscErrors.h"
#include "miscPrivate.h"


/**
 * Give back the network host name.
 *
 * In case the NULL-terminated hostname does not fit, no  error is returned, but
 * the hostname is truncated. It is unspecified whether the truncated hostname
 * will be NULL-terminated.
 *
 * \warning As is, this function uses 'uname()' SysV call, so it is not portable
 * to BSD-style systems that require 'gethostname()' system call.\n\n
 *
 * \param hostName allocated character array where the resulting date is stored
 * \param length allocated character array length
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT miscGetHostName(char *hostName, mcsUINT32 length)
{
    struct utsname systemInfo;

    /* Test 'hostName' parameter validity */
    if (hostName == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "hostName");
        return FAILURE;
    }

    /* Test 'length' parameter validity */
    if (length == 0)
    {
        errAdd(miscERR_NULL_PARAM, "length");
        return FAILURE;
    }

    /* Try to get the hostname from the system */
    if (uname(&systemInfo) != 0)
    {
        errAdd(miscERR_FUNC_CALL, "uname");
        return FAILURE;
    }

    if (strncpy((char *)hostName, systemInfo.nodename, length) == NULL)
    {
        errAdd(miscERR_FUNC_CALL, "strncpy");
        return FAILURE;
    }

    return SUCCESS;
}

/*___oOo___*/
