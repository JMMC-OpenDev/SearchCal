/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * \e \<msgSendCommand\> - program sending a command to a specified process, 
 * then waiting for a reply.
 *
 * \b Synopsis:
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
 * \b Details:
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

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgSendCommand.cpp,v 1.22 2007-02-22 12:27:59 gzins Exp $";
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

    // Disable message logging
    logSetStdoutLogLevel(logQUIET);

    // Read command-line options
    if (argc > 2)
    {
        if (strcmp(argv[1], "-v") == 0)
        {
            logSetStdoutLogLevel(logINFO);
            verbose = mcsTRUE;
            cnt += 1;
        }
    }

    if (((argc - cnt) == 3) || ((argc - cnt) == 4))
    {
        memset(process, '\0', sizeof(mcsPROCNAME));
        strncpy((char *)process, argv[cnt++], mcsPROCNAME_LEN);
        miscDeleteChr((char *)process, '\n', mcsTRUE);

        memset(command, '\0', sizeof(mcsCMD));
        strncpy((char *)command, argv[cnt++], mcsCMD_LEN);
        miscDeleteChr((char *)command, '\n', mcsTRUE);

        memset(params, '\0', sizeof(params));
        strncpy((char *)params, argv[cnt++], (sizeof(params) -1));
        miscDeleteChr((char *)params, '\n', mcsTRUE);

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

    // Try to connect to msgManager
    msgMANAGER_IF *manager = new msgMANAGER_IF;
    if (manager->Connect(argv[0]) == mcsFAILURE)
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    // Send the specified command to the specified process
    mcsINT32 cmdId;
    cmdId = manager->SendCommand(command, process, params);
    if (cmdId != mcsFAILURE)
    {
        if (verbose == mcsTRUE)
        {
            logInfo("Command sent, waiting for reply...\n");
        }

        // While last reply hasn't been received...
        while (manager->Receive(msg, timeout) == mcsSUCCESS)
        {
            // Test the received reply command name validity
            if (strcmp(command, msg.GetCommand()) != 0)
            {
                logWarning("Received '%s' command reply instead of '%s'\n",
                           command, msg.GetCommand());
            }

            // Test the reply type
            lastReply = msg.IsLastReply();
            switch (msg.GetType())
            {
                // Normal reply
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

                    // Error reply
                case msgTYPE_ERROR_REPLY:
                    if (verbose == mcsTRUE)
                    {
                        logInfo("Error reply received\n");
                    }
                    errUnpackStack(msg.GetBody(), msg.GetBodySize());
                    break;

                    // Unknown reply
                default:
                    errAdd(msgERR_MSG_TYPE, msg.GetType());
                    lastReply = mcsTRUE;
                    break;
            }

            // If the reply was the last...
            if (lastReply == mcsTRUE)
            {
                // Exit from the receiving loop
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
        // End of while() loop
    }
    else
    {
        if (verbose == mcsTRUE)
        {
            logInfo("Sending of command to '%s' failed...\n", process);
        }
    }

    // Print out error stack if it is not empty
    if (errStackIsEmpty() == mcsFALSE)
    {
        errCloseStack();
        status = EXIT_FAILURE;
    }

    // If it was not the command to stop msgManager
    if ((strcmp(command, "EXIT") != 0) || (strcmp(process, "msgManager") != 0))
    {
        // Disconnect from msgManager
        manager->Disconnect();
    }
    delete (manager);
    mcsExit();

    exit(status);
}


/*___oOo___*/
