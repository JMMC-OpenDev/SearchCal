/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgSendCommand.cpp,v 1.5 2004-12-03 08:52:23 gzins Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  16-Aug-2004  Ported from CILAS software
* lafrasse  19-Nov-2004  Used argv[0] instead of the hard-coded "msgSendCommand"
*                        value, and added the mcsExit() function call
* lafrasse  23-Nov-2004  Cleaned included headers
* gzins     29-Nov-2004  Fixed bug related to time-out handling
*                        Set default time-out to WAIT_FOREVER
* gzins     03-Dec-2004  Updated according to new msgMANAGER_IF::Connect API
*
*******************************************************************************/

/**
 * \file
 * \e \<msgSendCommand\> - 'msgManager' test program, sending a command,
 * then waiting its replies.
 *
 * \b Synopsis:\n
 * \e \<msgSendCommand\> [-v] \<process\> \<command\> \<commandPar\>
 * [\<time-out\>]
 *
 * \param process    : recepient process name
 * \param command    : name of the command to be sent
 * \param commandPar : command parameter list
 * \param time-out   : maximum waiting time-out, in milliseconds (by default,
 *                     wait forever)
 *
 * \optname v        : enable verbose mode
 *
 * \b Details:\n
 * \e \<msgSendCommand\> sends the \<command\> to the \<process\> process,
 * and then waits for the correspondant reply(ies). The received reply(ies) and
 * all possible errors are printed on stdout.
 *
 * The program will exit if :
 * \li the last reply is received
 * \li the \<time-out\> expired
 * \li an error occured
 *
 * \ex
 * msgSendCommand -v msgManager VERSION ""
 * 
 */

static char *rcsId="@(#) $Id: msgSendCommand.cpp,v 1.5 2004-12-03 08:52:23 gzins Exp $"; 
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
    mcsLOGICAL    verbose;
    mcsINT32      cnt;
    mcsPROCNAME   process;
    mcsCMD        command;
    msgMESSAGE    msg;
    mcsSTRING256  params;
    mcsINT32      timeout;
    mcsINT32      status;
    mcsLOGICAL    lastReply;

    mcsInit(argv[0]);
    errResetStack();

    verbose = mcsFALSE;
    cnt     = 1;
    timeout = msgWAIT_FOREVER;
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

        if ((argc - cnt) == 1)
        {
            timeout = atoi(argv[cnt]);
        }
    }
    else
    {
        printf("\nUsage : \tmsgSendCommand <process> <command> <commandPar> [<time-out>]\n");
        exit(EXIT_FAILURE);
    }

    /* Try to connect to msgManager */
    msgMANAGER_IF manager;
    if (manager.Connect(argv[0]) == FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Try to send the specified command to the specified process */
    if (manager.SendCommand(command, process, params, 0) == FAILURE)
    {
        goto exit;
    }
    else
    {
        if (verbose == mcsTRUE)
        {
            logInfo("Command sent, waiting for reply...\n");
        }
    }

    /* While last reply hasn't been received... */
    while (manager.Receive(msg, timeout) == SUCCESS)
    {
        /* Test the received reply command name validity */
        if (strcmp(command, msg.GetCommand()) != 0)
        {
            logWarning("Received '%s' command reply instead of '%s'\n",
                       command, msg.GetCommand());
        }

        /* Test the reply type */
        lastReply = msg.IsLastReply();
        switch (msg.GetType())
        {
            case msgTYPE_REPLY:
                if (verbose == mcsTRUE)
                {
                   logInfo("msgSendCommand : Normal reply received\n");
                }

                if (msg.GetBodySize() > 0)
                {
                    printf("MESSAGEBUFFER :\n%s\n", msg.GetBodyPtr());
                }
                break;

            case msgTYPE_ERROR_REPLY:
                errUnpackStack(msg.GetBodyPtr(), msg.GetBodySize());
                break;

            default:
                errAdd(msgERR_MSG_TYPE, msg.GetType());
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
                logInfo("msgSendCommand : Waiting next reply...\n");
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

    manager.Disconnect();

    mcsExit();
    exit(status);
}


/*___oOo___*/
