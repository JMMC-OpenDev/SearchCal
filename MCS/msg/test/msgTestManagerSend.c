/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgTestManagerSend.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  16-Aug-2004  Ported from CILAS software
*
*
*******************************************************************************/

/**
 * \file
 * \e \<msgTestManagerSend\> - 'msgManager' test program, sending a command,
 * then waiting its replies.
 *
 * \b Synopsis:\n
 * \e \<msgTestManagerSend\> [-v] \<process\> \<command\> \<commandPar\>
 * [\<time-out\>]
 *
 * \param process    : recepient process name
 * \param command    : name of the command to be sent
 * \param commandPar : command parameter list
 * \param time-out   : maximum waiting time-out, in milliseconds (by default,
 *                     there is no time-out)
 *
 * \optname v        : enable verbose mode
 *
 * \b Details:\n
 * \e \<msgTestManagerSend\> sends the \<command\> to the \<process\> process,
 * and then waits for the correspondant reply(ies). The received reply(ies) and
 * all possible errors are printed on stdout.
 *
 * The program will exit if :
 * \li the last reply is received
 * \li the \<time-out\> expired
 * \li an error occured
 *
 * \ex
 * msgTestManagerSend -v msgManager VERSION ""
 * 
 */

static char *rcsId="@(#) $Id: msgTestManagerSend.c,v 1.1 2004-08-24 15:01:53 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/*
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


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
 * Main
 */
int main (int argc, char *argv[])
{
    mcsLOGICAL  verbose;
    mcsINT32    cnt;
    mcsPROCNAME process;
    mcsCMD      command;
    msgMESSAGE  msg;
    mcsBYTES256 params;
    mcsINT32    timeout;
    mcsINT32    status;
    mcsLOGICAL  lastReply;

    errResetStack();
    verbose = mcsFALSE;
    cnt     = 1;
    timeout = msgNO_WAIT;
    status  = EXIT_SUCCESS;

    /* Try to read CLI Options */
    if (argc > 2)
    {
        if (strcmp(argv[1], "-v") == 0)
        {
            verbose = mcsTRUE;
            cnt += 1;
        }
    }

    if (((argc - cnt) == 3) || ((argc - cnt) == 4))
    {
        memset(process, '\0', sizeof(mcsPROCNAME));
        strncpy((char *)process, argv[cnt++], mcsPROCNAME_LEN);

        memset(command, '\0', sizeof(mcsCMD));
        strncpy((char *)command, argv[cnt++], mcsCMD_LEN);

        memset(params, '\0', sizeof(params));
        strncpy((char *)params, argv[cnt++], (sizeof(params) -1));

        if ((argc - cnt) == 4)
        {
            timeout = atoi(argv[cnt]);
        }
    }
    else
    {
        printf("\nUsage : \tmsgTestManagerSend <process> <command> <commandPar> [<time-out>]\n");
        exit(EXIT_FAILURE);
    }

    /* Try to connect to msgManager */
    if (msgConnect("msgTestManagerSend", NULL) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Try to send the specified command to the specified process */
    if (msgSendCommand(command, process, params, 0) == FAILURE)
    {
        goto exit;
    }
    else
    {
        if (verbose == mcsTRUE)
        {
            printf("msgTestManagerSend : Command sent, waiting for reply...\n");
        }
    }

    /* While last reply hasn't been received... */
    while (msgReceive(&msg, 1000) == SUCCESS)
    {
        /* Test the received reply command name validity */
        if (strcmp(command, msgGetCommand(&msg)) != 0)
        {
            printf("msgTestManagerSend : WARNING - Received '%s' command reply instead of '%s'\n",
                    command, msgGetCommand(&msg));
        }

        /* Test the reply type */
        lastReply = msgIsLastReply(&msg);
        switch (msgGetType(&msg))
        {
            case msgTYPE_REPLY:
                if (verbose == mcsTRUE)
                {
                   printf("msgTestManagerSend : Normal reply received\n");
                }

                if (msgGetBodySize(&msg) > 0)
                {
                    printf("MESSAGEBUFFER :\n%s\n", msgGetBodyPtr(&msg));
                }
                break;

            case msgTYPE_ERROR_REPLY:
                errUnpackStack(msgGetBodyPtr(&msg), msgGetBodySize(&msg));
                break;

            default:
                errAdd(msgERR_MSG_TYPE, msgGetType(&msg));
                lastReply = mcsTRUE;
                break;
        }

        /* If the reply was the last... */
        if (lastReply == mcsTRUE)
        {
            /* Exit from the receiving loog */
            break;
        }
        else
        {
            if(verbose)
            {
                printf("msgTestManagerSend : Waitng next reply...\n");
            }
        }
    }
    /* End of while() loop */

/* Try to disconnect from msgManager */
exit:
    if (errStackIsEmpty() == mcsFALSE)
    {
        errDisplayStack();
        errCloseStack();
        status = EXIT_FAILURE;
    }

    msgDisconnect();

    exit(status);
}


/*___oOo___*/
