/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgSocket.c,v 1.2 2004-11-22 14:21:20 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain all the TCP/IP socket management function definition to open/close.
 * 
 */

static char *rcsId="@(#) $Id: msgSocket.c,v 1.2 2004-11-22 14:21:20 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/* 
 * Local Headers
 */
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Return a created socket of the specified type, and give back its port number.
 *
 * The created socket can be of type SOCK_STREAM or SOCK_DGRAM, as specified
 * by the \<socketType\> parameter.
 *
 * \param portNumberPt the address of the given-beck port number
 * \param socketType an already connected socket to a msgManager
 *
 * \return the created socket identifier, or -1 if an error occured
 */
int             msgSocketCreate   (unsigned short     *portNumberPt,
                                   int                socketType)
{
    int                sd;
    struct sockaddr_in addr;
    mcsINT32           addrLen;

    logExtDbg("msgSocketCreate()");

    /* Try to create the socket */
    sd = socket(AF_INET, socketType, 0);
    if (sd == -1)
    { 
        errAdd(msgERR_SOCKET, strerror(errno));
        return -1; 
    }

    /* Try to bind the socket */
    memset((char *) &addr, 0, sizeof(addr));
    addr.sin_port        = htons(*portNumberPt);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family      = AF_INET;
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        errAdd(msgERR_BIND, strerror(errno));
        msgSocketClose(sd);
        return -1;
    }

    addrLen = sizeof(addr);

    /* Verify the new socket */
    if (getsockname(sd, (struct sockaddr *)&addr, &addrLen) == -1)
    {
        errAdd(msgERR_GETSOCKNAME, strerror(errno));
        msgSocketClose(sd);
        return -1;
    }

    /* Give back the soocket prot number in local host byte order */
    *portNumberPt = ntohs(addr.sin_port);

    /* Return the created socket identifier */
    return (sd);
}


/**
 * Close a specified socket.
 *
 * \param sd an already connected socket
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgSocketClose    (int                sd)
{
    logExtDbg("msgSocketClose()");

    (void) shutdown (sd, 2);
    (void) close (sd);

    return SUCCESS;
}


/*___oOo___*/
