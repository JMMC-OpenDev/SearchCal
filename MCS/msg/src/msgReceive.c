/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgReceive.c,v 1.3 2004-11-19 17:15:47 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
* lafrasse  07-Oct-2004  Added msgIsConnected
* lafrasse  19-Nov-2004  Changed msgMESSAGE structure name to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgReceive() and msgReceiveFrom() function definition, used to
 * receive messages from the 'msgManager' communication server.
 * 
 */

static char *rcsId="@(#) $Id: msgReceive.c,v 1.3 2004-11-19 17:15:47 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
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
#include "msg.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to receive a message from the communication server.
 *
 * Wait for a message receive from 'msgManager'.\n
 * The \<timeoutInMs\> can have specific values : msgWAIT_FOREVER or msgNO_WAIT.
 *
 * \param msg an already allocated message structure pointer
 * \param timeoutInMs a time out value in milliseconds
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgReceive        (msgMESSAGE_RAW     *msg,
                                   mcsINT32           timeoutInMs)
{
    logExtDbg("msgSendReply()");
    
    /* If no connection is already open... */
    if (msgIsConnected() == mcsFALSE)
    {
        /* Raise an error */
        errAdd(msgERR_PROC_NOT_CONNECTED);

        /* Return an errror code */
        return FAILURE;
    }

    /* Return weither a message was received or not */
    return (msgReceiveFrom(msgManagerSd, msg, timeoutInMs));
}


/**
 * Internal function to try to receive a message with a specific socket.
 *
 * The \<timeoutInMs\> can have specific values : msgWAIT_FOREVER or msgNO_WAIT.
 *
 * \param sd an already connected socket to a msgManager
 * \param msg an already allocated message structure pointer
 * \param timeoutInMs a time out value in milliseconds
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgReceiveFrom    (int                sd,
                                   msgMESSAGE_RAW     *msg,
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
    FD_SET(sd, &readMask);

    if (timeoutInMs != msgWAIT_FOREVER)
    {
        status = select(sd + 1, &readMask, NULL, NULL, &timeout);
    }
    else
    {
        status = select(sd + 1, &readMask, NULL, NULL, (struct timeval *)NULL);
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
    ioctl(sd, FIONREAD, (unsigned long *)&nbBytesToRead);
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
        nbBytesRead = recv(sd, (char *)&msg->header, sizeof(msgHEADER), 0);
        if (nbBytesRead != sizeof(msgHEADER))
        {
            errAdd(msgERR_PARTIAL_HDR_RECV,nbBytesRead, sizeof(msgHEADER));
            return (FAILURE);
        }

        /* Try to read the message body if it exists */
        memset(msgGetBodyPtr(msg), '\0', (msgBODYMAXLEN + 1));
        if (msgGetBodySize(msg) != 0 )
        {
            nbBytesRead = recv(sd, msgGetBodyPtr(msg), msgGetBodySize(msg), 0);
            if (nbBytesRead != msgGetBodySize(msg))
            {
                errAdd(msgERR_PARTIAL_BODY_RECV, nbBytesRead,
                       msgGetBodySize(msg));
                return (FAILURE); 
            }
        }
    } 

    return SUCCESS;
}


/*___oOo___*/
