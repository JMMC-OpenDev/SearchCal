/*******************************************************************************
* JMMC project
*
* "@(#) $Id: msgSOCKET.cpp,v 1.1 2004-11-19 17:19:45 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    19-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * msgSOCKET class definition.
 */

static char *rcsId="@(#) $Id: msgSOCKET.cpp,v 1.1 2004-11-19 17:19:45 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msgSOCKET.h"
#include "msgMESSAGE.h"
#include "msgPrivate.h"
#include "msgErrors.h"

/*
 * Class constructor
 */
msgSOCKET::msgSOCKET()
{
}

/*
 * Class destructor
 */
msgSOCKET::~msgSOCKET()
{
}

/*
 * Public methods
 */
/**
 * Get the socket number
 *
 * \return the socket number
 */
mcsINT32 msgSOCKET::GetSocketId()
{
    // return the socket number
    return _socketId;
}

/**
 * Open a socket 
 *
 * \param portNumberPt the address of the given-beck port number
 * \param socketType an already connected socket to a msgManager 
 *  
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 */
mcsCOMPL_STAT msgSOCKET::Open(unsigned short     *portNumberPt,
                                int                socketType)
{
    int                sd;
    struct sockaddr_in addr;
    mcsINT32           addrLen;

    logExtDbg("msgSOCKET::SocketCreate()");

    /* Create the socket */
    sd = socket(AF_INET, socketType, 0);
    if (sd == -1)
    { 
        errAdd(msgERR_SOCKET, strerror(errno));
        return FAILURE; 
    }

    /* Bind the socket */
    memset((char *) &addr, 0, sizeof(addr));
    addr.sin_port        = htons(*portNumberPt);
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_family      = AF_INET;
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        errAdd(msgERR_BIND, strerror(errno));
        Close();
        return FAILURE;
    }

    addrLen = sizeof(addr);

    /* Verify the new socket */
    if (getsockname(sd, (struct sockaddr *)&addr, (socklen_t*)&addrLen) == -1)
    {
        errAdd(msgERR_GETSOCKNAME, strerror(errno));
        Close();
        return FAILURE;
    }

    /* Give back the soocket prot number in local host byte order */
    *portNumberPt = ntohs(addr.sin_port);
    
    // Affect the created socket identifier
    _socketId = sd;

    return SUCCESS;
}

/**
 * Close the socket
 *
 * \return always SUCCESS
 */
mcsCOMPL_STAT msgSOCKET::Close()
{
    logExtDbg("msgSOCKET::socketClose()");

    (void) shutdown (_socketId, 2);
    (void) close (_socketId);

    return SUCCESS;
}

/**
 * Send a message on the socket 
 *
 * \param msg the message to send
 *
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Send(msgMESSAGE &msg)
{
    mcsINT32   msgLen;
    mcsINT32   nbBytesSent;

    logExtDbg("msgSOCKET::Send()");

    /* Try to send the message */
    msgLen = sizeof(msgHEADER) + msg.GetBodySize();
    nbBytesSent = send(_socketId, (char *)&msg, msgLen, 0);
    /* If some sent bytes were lost... */
    if (nbBytesSent != msgLen)
    {
        /* If no byte at all were sent... */
        if (nbBytesSent == 0)
        {
            errAdd(msgERR_SEND, strerror(errno));
        }
        else
        {
            errAdd(msgERR_PARTIAL_SEND, nbBytesSent, msgLen, strerror(errno));
        }

        /* Return an errror code */
        return FAILURE;
    }

    return SUCCESS;
}


/**
 *  
 *
 * \return SUCCESS on successfull completion otherwise FAILURE is return
 **/
mcsCOMPL_STAT msgSOCKET::Receive(msgMESSAGE         &msg,
                                 mcsINT32           timeoutInMs)
{
    struct timeval timeout ;
    fd_set         readMask ;
    mcsINT32       status, nbBytesToRead, nbBytesRead;

    logExtDbg("msgReceiveFrom()");

    /* Compute the timeout value */
    if (timeoutInMs == msgNO_WAIT)
    {
        timeout.tv_sec  = 0 ;
        timeout.tv_usec = 0 ;
    }
    else if (timeoutInMs != msgWAIT_FOREVER )
    {
        timeout.tv_sec  = (timeoutInMs / 1000);
        timeout.tv_usec = (timeoutInMs % 1000);
    }

    FD_ZERO(&readMask);
    FD_SET(_socketId, &readMask);

    if (timeoutInMs != msgWAIT_FOREVER)
    {
        status = select(_socketId + 1, &readMask, NULL, NULL, &timeout);
    }
    else
    {
        status = select(_socketId + 1, &readMask, NULL, NULL, (struct timeval *)NULL);
    }

    /* If the timeout expired... */ 
    if (status == 0)
    {
        /* Raise an error */
        errAdd(msgERR_TIMEOUT_EXPIRED, "No specific error message !!!");

        /* Return an error code */
        return FAILURE;
    }

    /* If an error occured during select() */
    if (status == -1)
    {
        /* Raise an error */
        errAdd(msgERR_SELECT, strerror(errno));

        /* Return an error code */
        return FAILURE;
    }

    /* If the connection with the remote processus was lost... */
    ioctl(_socketId, FIONREAD, (unsigned long *)&nbBytesToRead);
    if (nbBytesToRead == 0)
    {
        /* Raise an error */
        errAdd(msgERR_BROKEN_PIPE);

        /* Return an error code */
        return FAILURE;
    }
    else
    {
        /* Try to read the message header */
        nbBytesRead = recv(_socketId, (char *)msg.GetHeaderPtr(), sizeof(msgHEADER), 0);
        if (nbBytesRead != sizeof(msgHEADER))
        {
            errAdd(msgERR_PARTIAL_HDR_RECV,nbBytesRead, sizeof(msgHEADER));
            return (FAILURE);
        }

        /* Try to read the message body if it exists */
        memset(msg.GetBodyPtr(), '\0', (msgBODYMAXLEN + 1));
        if (msg.GetBodySize() != 0 )
        {
            nbBytesRead = recv(_socketId, msg.GetBodyPtr(), msg.GetBodySize(), 0);
            if (nbBytesRead != msg.GetBodySize())
            {
                errAdd(msgERR_PARTIAL_BODY_RECV, nbBytesRead,
                       msg.GetBodySize());
                return (FAILURE); 
            }
        }
    } 

    return SUCCESS;
}
/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
