/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msg.c,v 1.4 2004-10-01 13:05:41 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  11-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain all the message structure accessor functions.
 * 
 * \n \b Code \b Example:\n
 * \n A simple main using a Dynamic Buffer.
 * \code
 * int main (int argc, char *argv[])
 * {
 *     msgMESSAGE  msg;
 *     mcsINT32    status;
 *     mcsLOGICAL  lastReply;
 *
 *     errResetStack();
 *     status  = EXIT_SUCCESS;
 *     
 *     // Connection to msgManager
 *     if (msgConnect("myProgram", NULL) == FAILURE)
 *     {
 *         errDisplay();
 *         errCloseStack();
 *         exit(EXIT_FAILURE);
 *     }
 * 
 *     // Command sending
 *     if (msgSendCommand("MYCMD", "theServer", NULL, 0) == FAILURE)
 *     {
 *         errDisplay();
 *         errCloseStack();
 *         msgDisconnect();
 *         exit(EXIT_FAILURE);
 *     }
 *     
 *     // Answer(s) reading
 *     while (msgReceive(&msg, 1000) == SUCCESS)
 *     {
 *         // Answer type checking
 *         lastReply = msgIsLastReply(&msg);
 *         switch (msgGetType(&msg))
 *         {
 *             case msgTYPE_REPLY:
 *                 if (msgGetBodySize(&msg) > 0)
 *                 {
 *                     printf("MESSAGEBUFFER:\n%s\n", msgGetBodyPtr(&msg));
 *                 }
 *                 break;
 * 
 *             case msgTYPE_ERROR_REPLY:
 *                 errUnpackStack(msgGetBodyPtr(&msg), msgGetBodySize(&msg));
 *                 break;
 * 
 *             default:
 *                 errAdd(msgERR_MSG_TYPE, msgGetType(&msg));
 *                 lastReply = mcsTRUE;
 *                 break;               
 *         }
 *         
 *         // Receiving-loop exiting
 *         if (lastReply == mcsTRUE) 
 *         {
 *             break;
 *         }
 *     }
 * 
 *     // Error displaying
 *     if (errStackIsEmpty() == mcsFALSE)
 *     {
 *         errDisplay();
 *         errCloseStack();
 *         status = EXIT_FAILURE;
 *     }
 *     
 *     // Deconnection from msgManager
 *     msgDisconnect();
 *
 *     exit(status);
 * }
 * \endcode
 */

static char *rcsId="@(#) $Id: msg.c,v 1.4 2004-10-01 13:05:41 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

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

/*
 * Local Variables
 */
int msgManagerSd = -1;

/*
 * Public functions definition
 */

/**
 * Copy \<bufLen\> bytes of \<buffer\> in the message body.
 *
 * If \<bufLen\> equal 0, strlen() is used to get \<buffer\> length to be
 * copied in.
 * If \<buffer\> equal NULL, the message body is resetted with '\\0'.
 *
 * \param msg a message structure address
 * \param buffer the byte buffer to be copied in
 * \param bufLen the size to be copied in
 *
 * \return FAILURE if the to-be-copied-in byte number is greater than the
 * message body maximum size, SUCCESS otherwise
 */
mcsCOMPL_STAT   msgSetBody        (msgMESSAGE         *msg,
                                   char               *buffer,
                                   mcsINT32           bufLen)
{
    logExtDbg("msgSetBody()");
    
    /* Reset the message body with '\0' */
    memset(msg->body, '\0', sizeof(msg->body));

    /* If there is nothing to copy in... */
    if (buffer == NULL)
    {
        /* Set the to-be-copied-in byte number to 0 */
        bufLen = 0;
    }
    else 
    {
        /* If no to-be-copied-in byte number is given... */
        if (bufLen == 0)
        {
            /* Get the given buffer total length */
            bufLen = strlen(buffer);
        }
    }

    /* If the to-be-copied byte number is greater than the message body
     * maximum size...
     */
    if (bufLen > msgBODYMAXLEN)
    {
        /* Raise an error */
        errAdd(msgERR_BUFFER_TOO_BIG, bufLen, msgBODYMAXLEN);

        /* Return an error code */
        return FAILURE;
    }
    
    /* Store the new message body size, in network byte order */
    sprintf( msg->header.msgBodySize, "%d", bufLen);
    /* Fill the message body with the given length and buffer content */
    strncpy(msg->body, buffer, bufLen);
    
    return SUCCESS;
}


/**
 * Return the address of the message body.
 *
 * \param msg a message structure address
 *
 * \return the address of the message body
 */
char *          msgGetBodyPtr     (msgMESSAGE         *msg)
{
    /* Return the message body address */
    return (msg->body);
}


/**
 * Return the message body size.
 *
 * \param msg a message structure address
 *
 * \return the message body size
 */
mcsINT32        msgGetBodySize    (msgMESSAGE         *msg)
{
    mcsINT32 msgBodySize;
    /* Return the message body size in local host byte order */
    sscanf(msg->header.msgBodySize, "%d", &msgBodySize);
    return (msgBodySize);
}


/**
 * Return the address of the message command name.
 *
 * \param msg a message structure address
 *
 * \return the address of the message command name
 */
char *          msgGetCommand     (msgMESSAGE         *msg)
{
    /* Return the message command name address */
    return (msg->header.command);
}


/**
 * Return the address of the message sender processus name.
 *
 * \param msg a message structure address
 *
 * \return the address of the message sender processus name
 */
char *          msgGetSender      (msgMESSAGE         *msg)
{
    /* Return the message sender processus name address */
    return (msg->header.sender);
}


/**
 * Return the address of the message receiver processus name.
 *
 * \param msg a message structure address
 *
 * \return the address of the message receiver processus name
 */
char *          msgGetRecipient   (msgMESSAGE         *msg)
{
    /* Return the message receiver processus name address */
    return (msg->header.recipient);
}


/**
 * Return weither it is the last message or not.
 *
 * \param msg a message structure address
 *
 * \return mcsTRUE or mcsFALSE
 */
mcsLOGICAL      msgIsLastReply    (msgMESSAGE         *msg)
{
    /* Return weither it is the last message or not */
    return (msg->header.lastReply);
}

/**
 * Return the message type.
 *
 * \param msg a message structure address
 *
 * \return the message type
 */
msgTYPE         msgGetType        (msgMESSAGE         *msg)
{
    /* Return the message type */
    return (msg->header.type);
}


/*___oOo___*/
