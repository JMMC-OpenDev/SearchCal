/*******************************************************************************
 * JMMC project
 * 
 * "@(#) $Id: msgSendCommand.cpp,v 1.14 2005-02-04 15:57:06 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2005/02/03 11:09:24  gzins
 * Returned EXIT_FAILURE when error reply is received
 *
 * Revision 1.12  2005/01/29 19:59:42  gzins
 * Minor change in file history
 *
 * Revision 1.11  2005/01/26 08:43:50  gzins
 * Suppressed useless 'MESSAGEBUFFER:' ouput when printing out command reply.
 *
 * Revision 1.10  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 *                        Displayed error stack when receiving error reply
 * gzins     22-Dec-2004  Renamed GetBodyPtr to GetBody
 * gzins     20-Dec-2004  Removed leading and trailing spaces to parameters
 * gzins     07-Dec-2004  Removed no longer needed errStackDisplay() 
 * gzins     03-Dec-2004  Updated according to new msgMANAGER_IF::Connect API
 * gzins     29-Nov-2004  Fixed bug related to time-out handling
 *                        Set default time-out to WAIT_FOREVER
 * lafrasse  23-Nov-2004  Cleaned included headers
 * lafrasse  19-Nov-2004  Used argv[0] instead of the hard-coded
 *                        "msgSendCommand" value, and added the mcsExit()
 *                        function call
 * lafrasse  16-Aug-2004  Ported from CILAS software
 *
 ******************************************************************************/

/**
 * \file
 * \e \<msgSendCommand\> - program sending a command to a specified process, 
 * then waiting for a reply.
 *
 * \b Synopsis:\n
 * \e \<msgSendCommand\> [-v] \<process\> \<command\> \<commandPar\>
 * [\<time-out\>]
 *
 * \param process    : recepient process name
 * \param command    : name of the command to be sent
 * \param commandPar : command parameter list
 * \param time-out   : maximum waiting time-out, in milliseconds (waits forever
 *                     if not specified)
 *
 * \optname v        : enable verbose mode
 *
 * \b Details:\n
 * \e \<msgSendCommand\> sends the \<command\> command to the \<process\>
 * process, and then waits for the corresponding reply(ies). The received
 * reply(ies) and all possible errors are then printed on the standard output.
 *
 * The program will exit when :
 * \li the last reply of the dialog is received;
 * \li the \<time-out\> expired;
 * \li an error occured.
 *
 * \ex
 * msgSendCommand -v msgManager VERSION ""
 * 
 */

static char *rcsId="@(#) $Id: msgSendCommand.cpp,v 1.14 2005-02-04 15:57:06 lafrasse Exp $"; 
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

    /* Read command-line options */
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
        // Removed leading and trailing space
        miscTrimString(params, " ");

        if ((argc - cnt) == 1)
        {
            timeout = atoi(argv[cnt]);
        }
    }
    else
    {
        printf("Usage : \tmsgSendCommand <process> <command> <commandPar> [<time-out>]\n");
        exit(EXIT_FAILURE);
    }

    /* Try to connect to msgManager */
    msgMANAGER_IF manager;
    if (manager.Connect(argv[0]) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Send the specified command to the specified process */
    mcsINT32 cmdId;
    cmdId = manager.SendCommand(command, process, params);
    if (cmdId != mcsFAILURE)
    {
        if (verbose == mcsTRUE)
        {
            logInfo("Command sent, waiting for reply...\n");
        }

        /* While last reply hasn't been received... */
        while (manager.Receive(msg, timeout) == mcsSUCCESS)
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
                /* Normal reply */
                case msgTYPE_REPLY:
                    if (verbose == mcsTRUE)
                    {
                        logInfo("Normal reply received\n");
                    }

                    if (msg.GetBodySize() > 0)
                    {
                        if (verbose == mcsTRUE)
                        {
                            printf("MESSAGEBUFFER :\n");
                        }
                        printf("%s\n", msg.GetBody());
                    }
                    break;

                    /* Error reply */
                case msgTYPE_ERROR_REPLY:
                    if (verbose == mcsTRUE)
                    {
                        logInfo("Error reply received\n");
                    }
                    errUnpackStack(msg.GetBody(), msg.GetBodySize());
                    break;

                    /* Unknown reply */
                default:
                    errAdd(msgERR_MSG_TYPE, msg.GetType());
                    lastReply = mcsTRUE;
                    break;
            }

            /* If the reply was the last... */
            if (lastReply == mcsTRUE)
            {
                /* Exit from the receiving loop */
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
    }
    else
    {
        if (verbose == mcsTRUE)
        {
            logInfo("Sending of command to '%s' failed...\n", process);
        }
    }

    /* Print out error stack if it is not empty */
    if (errStackIsEmpty() == mcsFALSE)
    {
        errCloseStack();
        status = EXIT_FAILURE;
    }

    /* Disconnect from msgManager */
    manager.Disconnect();

    mcsExit();
    exit(status);
}


/*___oOo___*/
