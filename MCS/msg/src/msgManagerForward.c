/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgManagerForward.c,v 1.2 2004-11-19 17:15:47 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
* lafrasse  19-Nov-2004  Changed msgMESSAGE structure name to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgManagerForwardCmd() and msgManagerForwardReply() function definition, used
 * to pass messages or answers to their recipient process.
 * 
 */

static char *rcsId="@(#) $Id: msgManagerForward.c,v 1.2 2004-11-19 17:15:47 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/*
 * System Headers
 */
#include <stdio.h>


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
#include "msgManager.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to pass a command to its recepient process.
 *
 * \param msg the message to be forwarded
 * \param process the recipient process name
 * \param procList the process list in which looking at the recipient process
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerForwardCmd      (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList)
{
    msgPROCESS *recipient;

    logExtDbg("msgManagerForwardCmd()");
    logInfo("Received '%s' command from '%s' for '%s'", msgGetCommand(msg),
            msgGetSender(msg), msgGetRecipient(msg));

    /* Try to find the recipient process in the process list */
    recipient = msgManagerProcessListFindByName(procList, msgGetRecipient(msg));
    if (recipient == NULL)
    {
        /* Raise the error to the sender */
        errAdd(msgERR_RECIPIENT_NOT_CONNECTED, msgGetRecipient(msg),
               msgGetCommand(msg));
        if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
        {
            errCloseStack();
        }
    }
    else
    {
        /* If the command could not be delivered to the recipient process... */
        if (msgSendTo(recipient->sd, msg) == FAILURE)
        {
            /* Try to report this to the sender */
            if (msgSendReplyTo(process->sd, msg, mcsTRUE) == FAILURE)
            {
                errCloseStack();
            }
            return FAILURE;
        }
    }
       
    return SUCCESS;
}


/**
 * Try to pass a reply to its recepient process.
 *
 * \param msg the message to be forwarded
 * \param process the recipient process name
 * \param procList the process list in which looking at the recipient process
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerForwardReply    (msgMESSAGE_RAW     *msg,
                                           msgPROCESS         *process,
                                           msgPROCESS_LIST    *procList)
{
    msgPROCESS      *sender;

    logExtDbg("msgManagerForwardReply()");
    logInfo("Received '%s' reply from '%s' for '%s'", msgGetCommand(msg),
            msgGetRecipient(msg), msgGetSender(msg));

    /* Try to find the recipient process in the process list */
    sender = msgManagerProcessListFindByName(procList, msgGetSender(msg));
    if (sender == NULL)
    {
        /* Raise the error to the replyer */
        errAdd(msgERR_SENDER_NOT_CONNECTED, msgGetSender(msg),
               msgGetCommand(msg));
        return FAILURE;
    }
    else
    {
        /* Try to deliver the reply */
        if (msgSendTo(sender->sd, msg) == FAILURE)
        {
            return FAILURE;
        }
    }
       
    return SUCCESS;
}


/*___oOo___*/
