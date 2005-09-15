/*******************************************************************************
 * JMMC project
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/05/23 11:57:40  lafrasse
 * Code review : user documentation refinments
 *
 * Revision 1.3  2005/01/28 18:39:10  gzins
 * Changed FAILURE/SUCCESS to mcsFAILURE/mscSUCCESS
 *
 * lafrasse  03-Aug-2004  Created
 *
 *----------------------------------------------------------------------------*/

/**
 * @file
 * Declaration of miscNetwork functions.
 */

static char *rcsId="@(#) $Id: miscNetwork.c,v 1.5 2005-09-15 14:19:07 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/in.h>

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
 * Give back the local machine network host name as a null-terminated string.
 *
 * In case the null-terminated host name string does not fit entirely in the
 * given external buffer, no error is returned and the host name become
 * truncated. It is then unspecified whether the truncated host name will be
 * null-terminated or not.
 *
 * @warning As is, this function uses the 'uname()' SysV call, so it is not
 * directly portable to BSD-style systems that may require 'gethostname()'
 * system call instead.\n\n
 *
 * @param hostName address of the receiving, already allocated extern buffer in
 * which the host name will be stored.
 * @param length maximum extern buffer capacity.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetHostName(char *hostName, const mcsUINT32 length)
{
    struct utsname systemInfo;

    /* Test 'hostName' parameter validity */
    if (hostName == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "hostName");
        return mcsFAILURE;
    }

    /* Test 'length' parameter validity */
    if (length == 0)
    {
        errAdd(miscERR_NULL_PARAM, "length");
        return mcsFAILURE;
    }

    /* Try to get the hostname from the system */
    if (uname(&systemInfo) != 0)
    {
        errAdd(miscERR_FUNC_CALL, "uname");
        return mcsFAILURE;
    }

    /* Give back the found hostname */
    strncpy(hostName, systemInfo.nodename, length);

    return mcsSUCCESS;
}

/**
 * Give host ip address from the name
 *
 * @param ipAddress the IP address to find
 * @param hostName the name of the host
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscGetHostByName(char *ipAddress, const char *hostName)
{

    /* Structure used to resolved host name to IP*/
    struct hostent *hostStructure = gethostbyname(hostName);
    struct in_addr a;

    /* if an error occur */
    if (hostStructure == NULL)
    {
        if (h_errno == HOST_NOT_FOUND)
        {
            errAdd(miscERR_HOST_NOT_FOUND, hostName);
            return mcsFAILURE;
        }
        else if ((h_errno == NO_ADDRESS) || (h_errno == NO_ADDRESS))
        {
            errAdd(miscERR_NO_ADDRESS, hostName);
            return mcsFAILURE;
        }
        else if (h_errno == NO_RECOVERY)
        {
            errAdd(miscERR_NO_RECOVERY);
            return mcsFAILURE;
        }
        else if (h_errno == TRY_AGAIN)
        {
            errAdd(miscERR_TRY_AGAIN);
            return mcsFAILURE;
        }
    }

    /* Get IP address */
    while (*hostStructure->h_addr_list != NULL)
    {
        memcpy((char *) &a, *hostStructure->h_addr_list++, sizeof(a));
    }

    /* copy ip in the resulting ip address */
    strcpy(ipAddress, inet_ntoa(a));
    return mcsSUCCESS;
}

/*___oOo___*/
