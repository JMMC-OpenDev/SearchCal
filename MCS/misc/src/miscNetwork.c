/*******************************************************************************
 * JMMC project
 *----------------------------------------------------------------------------*/

/**
 * @file
 * Declaration of miscNetwork functions.
 */



/* Needed to preclude warnings on snprintf(), popen() and pclose() */
#define  _DEFAULT_SOURCE 1

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
#include <ctype.h>
#include <unistd.h>


/*
 * MCS Headers
 */
#include "err.h"
#include "log.h"


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
        memcpy((char *) &address, *hostStructure->h_addr_list++, sizeof (address));
    }

    /* copy ip in the resulting ip address */
    strcpy(ipAddress, inet_ntoa(address));
    return mcsSUCCESS;
}

/**
 * Perform the given request as an HTTP POST.
 *
 * This method ensures proper handling of HTTP redirections and proxies.
 *
 * @warning As is, this function uses the 'curl' command-line utility, so it is
 * not directly portable without this dependency.\n\n
 *
 * @param uri the HTTP request that should be performed
 *   (eg. http://site.org/script.php?).
 * @param data the POST data that should be performed
 *   (eg. p1=v1&p2=v2).
 * @param outputBuffer address of the receiving, already allocated dynamic buffer
 * in which the query result will be stored.
 * @param timeout maximum connection timeout (in seconds, 30 if 0 is given).
 *
 * @return 0 on successful completion. Otherwise the return code as 8-bits integer is returned.
 */
mcsINT8 miscPerformHttpPost(const char *uri, const char *data, miscDYN_BUF *outputBuffer, const mcsUINT32 timeout)
{
    /* Test 'uri' parameter validity */
    if (uri == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "uri");
        return mcsFAILURE;
    }

    /* Test 'data' parameter validity */
    if (data == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "data");
        return mcsFAILURE;
    }

    /* Test 'outputBuffer' parameter validity */
    if (outputBuffer == NULL)
    {
        errAdd(miscERR_NULL_PARAM, "outputBuffer");
        return mcsFAILURE;
    }

    /* 30sec timeout, -s makes curl silent, -S reports errors, -L indicates HTTP location */
    mcsUINT32 internalTimeout = (timeout > 0 ? timeout : 30);

    /* disable redirects with HTTP POST as not well supported (Violate RFC 2616/10.3.3 and switch from POST to GET) */
    static const char* staticCommand = "/usr/bin/curl --max-redirs 0 --max-time %d --retry 3 -S -s -L \"%s\" -d \"%s\"";

    int composedCommandLength = strlen(staticCommand) + strlen(uri) + strlen(data) + 10 + 1;

    /* Forging the command */
    char* composedCommand = (char*) malloc(composedCommandLength * sizeof (char));
    if (composedCommand == NULL)
    {
        errAdd(miscERR_ALLOC);
        return mcsFAILURE;
    }
    snprintf(composedCommand, composedCommandLength, staticCommand, internalTimeout, uri, data);

    /* retry up to 3 times to avoid http errors */
    mcsINT8 executionStatus = mcsFAILURE;
    mcsUINT32 tryCount = 0;
    mcsUINT32 waitDuration = 10;

    do
    {
        /* Erase the error stack */
        errResetStack();

        /* sleep 3 seconds before retrying query */
        if (tryCount != 0)
        {
            logInfo("Waiting %ds before retrying...", waitDuration);
            sleep(waitDuration);
            waitDuration *= 2;
            logInfo("Retrying HTTP POST (exec status = %d)", executionStatus);
        }

        /* Executing the command */
        executionStatus = miscDynBufExecuteCommand(outputBuffer, composedCommand);

        tryCount++;
    }
        /* 47     Too many redirects. When following redirects, curl hit the maximum amount. */
    while (((executionStatus != 0) && (executionStatus != 47)) && (tryCount < 3));

    /* Give back local dynamically-allocated memory */
    free(composedCommand);

    return executionStatus;
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
 * @param outputBuffer address of the receiving, already allocated dynamic buffer
 * in which the query result will be stored.
 * @param timeout maximum connection timeout (in seconds, 30 if 0 is given).
 *
 * @return 0 on successful completion. Otherwise the return code as 8-bits integer is returned.
 */
mcsINT8 miscPerformHttpGet(const char *uri, miscDYN_BUF *outputBuffer, const mcsUINT32 timeout)
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

    /* 30sec timeout, -s makes curl silent, -L handle HTTP redirections */
    mcsUINT32 internalTimeout = (timeout > 0 ? timeout : 30);
    const char* staticCommand = "/usr/bin/curl --max-time %d --retry 3 -s -L \"%s\"";
    int composedCommandLength = strlen(staticCommand) + strlen(uri) + 10 + 1;
    /* Forging the command */
    char* composedCommand = (char*) malloc(composedCommandLength * sizeof (char));
    if (composedCommand == NULL)
    {
        errAdd(miscERR_ALLOC);
        return mcsFAILURE;
    }
    snprintf(composedCommand, composedCommandLength, staticCommand,
             internalTimeout, uri);

    /* Executing the command */
    mcsINT8 executionStatus = miscDynBufExecuteCommand(outputBuffer, composedCommand);

    /* Give back local dynamically-allocated memory */
    free(composedCommand);

    return executionStatus;
}

/* Converts a hex character to its integer value */
char from_hex(char ch)
{
    return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code)
{
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

/* Returns a url-encoded version of str */

/* IMPORTANT: be sure to free() the returned string after use */
char *miscUrlEncode(const char *str)
{
    if ( str == NULL )
    {
        return NULL;
    }
    const char *pstr = str;
    char *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
    while (*pstr)
    {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
            *pbuf++ = *pstr;
        else if (*pstr == ' ')
            *pbuf++ = '+';
        else
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}

/* Returns a url-decoded version of str */

/* IMPORTANT: be sure to free() the returned string after use */
char *miscUrlDecode(const char *str)
{
    if ( str == NULL )
    {
        return NULL;
    }
    const char *pstr = str;
    char *buf = malloc(strlen(str) + 1), *pbuf = buf;
    while (*pstr)
    {
        if (*pstr == '%')
        {
            if (pstr[1] && pstr[2])
            {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        }
        else if (*pstr == '+')
        {
            *pbuf++ = ' ';
        }
        else
        {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
    return buf;
}


/*___oOo___*/
