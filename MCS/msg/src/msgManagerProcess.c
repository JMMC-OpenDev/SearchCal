/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgManagerProcess.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * Contain the msgManagerProcessSetConnection() function definition, use to establish
 * the communication during connection.
 * 
 */

static char *rcsId="@(#) $Id: msgManagerProcess.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers
 */
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
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
#include "msgManager.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/**
 * Try to establish communication during connection with a process.
 *
 * Verify that the new process name is unic, otherwise reject the connection
 * request.
 *
 * \param connectionSocket the socket used to estabmish the connection
 * \param procList the process list in which looking at the recipient process
 *
 * \return an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT   msgManagerProcessSetConnection(
                                           int                 connectionSocket,
                                           msgPROCESS_LIST    *procList)
{
    mcsINT32        addrLen;
    struct sockaddr addr;
    int             sd;

    logExtDbg("msgManagerProcessSetConnection()");

    /* Try to accept the new connection */
    addrLen = sizeof(addr) ;
    sd = accept(connectionSocket, &addr, &addrLen);
    if (sd == -1)
    {
        logWarning("ERROR : accept() failed - %s\n", strerror(errno));
    }
    else
    {
        /* Try to receive the registering command */
        msgMESSAGE msg;
        if (msgReceiveFrom(sd, &msg, 1000) == FAILURE)
        {
            /* Close the connection */
            msgSocketClose(sd);
            errCloseStack();
        }
        else
        {
            /* If the registering command is received... */
            if (strcmp(msgGetCommand(&msg), msgREGISTER_CMD) == 0)
            {
                /* If a similarly named process is already registered... */
                if (msgManagerProcessListFindByName(procList,msgGetSender(&msg))
                    != NULL)
                {
                    logWarning(" - process already registered",
                               "'%s' connection refused", msgGetSender(&msg));

                    /* Give back the error the requesting process */
                    errAdd(msgERR_DUPLICATE_PROC, msgGetSender(&msg));
                    if (msgSendReplyTo(sd, &msg, mcsTRUE) == FAILURE)
                    {
                        errCloseStack();
                    }

                    /* Close the connection */
                    msgSocketClose(sd);
                }
                else
                {
                    /* Try to send a registering validation messsage */
                    msgSetBody(&msg, "OK", 0);
                    if (msgSendReplyTo(sd, &msg, mcsTRUE) == FAILURE)
                    {
                        errCloseStack();
                    }
                    else
                    {
                        logInfo("'%s' connection accepted", msgGetSender(&msg));

                        /* Try to add the new process to the process list */
                        if (msgManagerProcessListAdd(procList,
                                                     msgGetSender(&msg), sd)
                            == FAILURE)
                        {
                            errCloseStack();
                        }
                    }
                }
            }
            else /* Wrong message received... */
            {
                logWarning("Received a '%s' message instead of '%s'",
                        "- '%s' process connection refused",
                        msgGetCommand(&msg), msgREGISTER_CMD,
                        msgGetSender(&msg));

                msgSocketClose (sd);
            }
        }
    }

    return SUCCESS;
}


/*___oOo___*/
