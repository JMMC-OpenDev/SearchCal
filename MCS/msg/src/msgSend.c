/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgSend.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgSendXxx() functions definition, used to send all the different
 * kind of messages to the 'msgManager' communication server.
 * 
 */

static char *rcsId="@(#) $Id: msgSend.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
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
 * Try to send a command message to the communication server.
 *
 * Send the \<command\> named message with all the parameters stored in
 * \<buffer\>  to the \<destProc\> named process. If \<buflen\> equals 0, then
 * strlen() is used to get \<buffer\> size.
 *
 * \param command a command name
 * \param destProc a remote process name
 * \param buffer a parameter list stored in a string
 * \param bufLen the length of the parameter list string, or 0 to use strlen()
 * internally
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgSendCommand    (const char         *command,
                                   const mcsPROCNAME  destProc,
                                   const char         *buffer,  
                                   mcsINT32           bufLen)
{
    msgMESSAGE msg;

    logExtDbg("msgSendCommand()");

    /* If no connection is already open... */
    if (msgManagerSd == -1)
    {
        /* Raise an error */
        errAdd(msgERR_PROC_NOT_CONNECTED);

        /* Return an errror code */
        return FAILURE;
    }

    /* Build the message header */
    memset(&msg.header, '\0', sizeof(msgHEADER));
    strncpy(msg.header.sender, mcsGetProcName(), sizeof(mcsPROCNAME)-1);
    strncpy(msg.header.recipient, destProc, sizeof(mcsPROCNAME)-1);
    msg.header.type = msgTYPE_COMMAND;
    strncpy(msg.header.command, command, sizeof(mcsCMD)-1);
 
    /* Try to build the message body */
    if (msgSetBody(&msg, (char*)buffer, bufLen) == FAILURE)
    {
        return FAILURE;
    }

    /* Try to send the message */
    return (msgSendTo(msgManagerSd, &msg));
}


/**
 * Internal function to try to send a message with a specified socket.
 *
 * \param sd an already connected socket to a msgManager
 * \param msg a message structure address
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgSendTo         (int                sd,
                                   msgMESSAGE         *msg)
{
    mcsINT32   msgLen;
    mcsINT32   nbBytesSent;

    logExtDbg("msgSendTo()");

    /* Try to send the message */
    msgLen = sizeof(msgHEADER) + msgGetBodySize(msg);
    nbBytesSent = send(sd, (char *)msg, msgLen, 0);
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
 * Try to send a reply message to the communication server.
 *
 * \param msg a message structure address
 * \param lastReply flag to specify if the current messag is the last one or not
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgSendReply      (msgMESSAGE         *msg,
                                   mcsLOGICAL         lastReply)
{
    logExtDbg("msgSendReply()");

    /* If no connection is already open... */
    if (msgManagerSd == -1)
    {
        /* Raise an error */
        errAdd(msgERR_PROC_NOT_CONNECTED);

        /* Return an errror code */
        return FAILURE;
    }

    /* Try to send the reply message */
    return (msgSendReplyTo(msgManagerSd, msg, lastReply));
}


/**
 * Internal function to try to send a reply message with a specified socket.
 *
 * \param sd an already connected socket to a msgManager
 * \param msg a message structure address
 * \param lastReply flag to specify if the current messag is the last one or not
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgSendReplyTo    (int                sd,
                                   msgMESSAGE         *msg,
                                   mcsLOGICAL         lastReply)
{
    mcsINT32   msgLen;
    mcsINT32   nbBytesSent;

    logExtDbg("msgSendReplyTo()");

    /* Build the reply message header */
    msg->header.lastReply = lastReply;
    /* If there is no error in the MCS error stack */
    if (errStackIsEmpty() == mcsTRUE)
    {
        /* Set message type to REPLY */
        msg->header.type = msgTYPE_REPLY;
    }
    else
    {
        /* Try to put the MCS error stack data in the message body */
        if (errPackStack(msg->body, sizeof(msg->body)) == FAILURE)
        {
            return FAILURE;
        }

        /* Store the message body size in network byte order */
        msg->header.msgBodySize = htonl(strlen(msg->body));

        /* Set message type to ERROR_REPLY */
        msg->header.type = msgTYPE_ERROR_REPLY;

        /* Empty MCS error stack */
        errResetStack();
    }

    /* Try to send the message */
    logTest("Sending '%s' answer : %s", msgGetCommand(msg), msg->body);
    msgLen = sizeof(msgHEADER) + msgGetBodySize(msg);
    nbBytesSent = send(sd, (char *)msg, msgLen, 0);
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


/*___oOo___*/
