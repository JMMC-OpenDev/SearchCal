/*******************************************************************************
 * JMMC project
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2006/01/10 14:40:39  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.5  2005/09/15 14:19:07  scetre
 * Added miscGetHostByName in the miscNetwork file
 *
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

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: miscNetwork.c,v 1.7 2008-04-04 12:30:04 lafrasse Exp $"; 

/* Needed to preclude warnings on snprintf(), popen() and pclose() */
#define  _BSD_SOURCE 1

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <errno.h>
#include <sys/wait.h>
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

    /* Structure used to resolved host name to IP */
    struct hostent *hostStructure = gethostbyname(hostName);
    struct in_addr address;

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
        memcpy((char *) &address, *hostStructure->h_addr_list++, sizeof(address));
    }

    /* copy ip in the resulting ip address */
    strcpy(ipAddress, inet_ntoa(address));
    return mcsSUCCESS;
}

/**
 * Perform the given request as an HTTP GET.
 *
 * This method ensures proper handling of HTTP redirections and proxies.
 *
 * @warning As is, this function uses the 'curl' command-line utility, so it is
 * not directly portable without this dependency.\n\n
 *
 * @param uri the HTTP request that should be performed (eg. http://apple.com).
 * @param outputBuffer address of the receiving, already allocated extern buffer
 * in which the query result will be stored.
 * @param availableMemory maximum output buffer capacity.
 * @param timeout maximum connection timeout (in seconds, 30 if 0 is given).
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT miscPerformHttpGet(const char *uri, char *outputBuffer, const mcsUINT32 availableMemory, const mcsUINT32 timeout)
{
    /* Test 'uri' parameter validity */
    if (uri == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "uri");
        return mcsFAILURE;
    }

    /* Test 'outputBuffer' parameter validity */
    if (outputBuffer == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "outputBuffer");
        return mcsFAILURE;
    }

    /* Test 'availableMemory' parameter validity */
    if (availableMemory == 0)
    {
        errAdd(miscERR_NULL_PARAM, "availableMemory");
        return mcsFAILURE;
    }

    /* 30sec timeout, -s makes curl silent, -L handle HTTP redirections */
    mcsUINT32 internalTimeout = (timeout > 0 ? timeout : 30);
    const char* staticCommand = "/usr/bin/curl --max-time %d -s -L \"%s\"";
    int composedCommandLength = strlen(staticCommand) + strlen(uri) + 10 + 1;
    /* Forging the command */
    char* composedCommand = (char*)malloc(composedCommandLength * sizeof(char));
    if (composedCommand == NULL)
    {
        errAdd(miscERR_ALLOC);
        return mcsFAILURE;
    }
    snprintf(composedCommand, composedCommandLength, staticCommand,
             internalTimeout, uri);

    /* Executing the command */
    FILE* process = popen(composedCommand, "r");

    /* Keep reading command result, until an error occurs */
    int totalReadSize = 0;
    while (feof(process) == 0)
    {
        /* While buffer is not full yet */
        if (totalReadSize < availableMemory)
        {
            /* Write the command result in the buffer */
            totalReadSize += fread(outputBuffer, 1, availableMemory, process);
        }
        else /* Once the buffer has been fulfiled entirely */
        {
            /* Keep reading the result in a temporary buffer, to count needed
               memory space for later error message */
            mcsSTRING1024 tmp;
            totalReadSize += fread(tmp, 1, sizeof(tmp), process);
        }
    }
    int pcloseStatus = pclose(process);

    /* Give back local dynamically-allocated memory */
    free(composedCommand);

    /* Buffer overflow check */
    if (totalReadSize >= availableMemory)
    {
        errAdd(miscERR_BUFFER_OVERFLOW, availableMemory, totalReadSize);
        return mcsFAILURE;
    }

    /* pclose() status check */
    if (pcloseStatus == -1)
    {
        errAdd(miscERR_FUNC_CALL, "pclose", strerror(errno));
        return mcsFAILURE;
    }
    else
    {
        /* curl exec status check */
        int curlStatus = WEXITSTATUS(pcloseStatus);
        if (curlStatus != 0)
        {
            errAdd(miscERR_CURL_STATUS, curlStatus);
            return mcsFAILURE;
        }
    }

    return mcsSUCCESS;
}

/*___oOo___*/
