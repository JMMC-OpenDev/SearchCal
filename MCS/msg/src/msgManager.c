/*******************************************************************************
* JMMC project
* 
* "@(#) $Id: msgManager.c,v 1.3 2004-11-22 14:17:18 gzins Exp $"
*
*
* who       when                 what
* --------  -----------  -------------------------------------------------------
* lafrasse  13-Aug-2004  Ported from CILAS software
* lafrasse  19-Nov-2004  Changed msgMESSAGE structure name to msgMESSAGE_RAW
*
*
*******************************************************************************/

/**
 * \file
 * \e \<msgManager\> - inter-process network communication server.
 *
 * \b Synopsis:\n
 * \e \<msgManager\>
 *
 * \b Details:\n
 * \e \<msgManager\> is the communication server allowing message exchange
 * between processes. Each process connected to this server can send message to
 * the other connected processes.
 * 
 */

static char *rcsId="@(#) $Id: msgManager.c,v 1.3 2004-11-22 14:17:18 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>
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
#include "msgMESSAGE.h"
#include "msgManager.h"
#include "msgPrivate.h"
#include "msgErrors.h"


/* 
 * Signal catching functions  
 */

/**
 * Trap certain system signals.
 *
 * Particulary manages 'broken pipe' and 'dead process' signals, plus the end
 * signal (i.e. CTRL-C).
 *
 * \param signalNumber the system signal to be trapped
 */
void            msgSignalHandler          (int                signalNumber)
{
    logInfo("Received %d system signal...", signalNumber);
    logInfo("msgManager programm aborted.");
    exit (EXIT_SUCCESS);
}


/* 
 * Main
 */
int main (int argc, char *argv[])
{
    msgPROCESS_LIST procList;
    int             connectionSocket;
    unsigned short  connectionPortNumber;
    fd_set          readMask, readReferenceMask;
    
    mcsInit("msgManager");
    errResetStack();

    logInfo("Program starting ...");



    /* Init system signal trapping */
    if (signal(SIGINT, msgSignalHandler) == SIG_ERR)
    {
        logWarning("signal(SIGINT, ...) function error");
        exit(EXIT_FAILURE);
    }
    if (signal (SIGTERM, msgSignalHandler) == SIG_ERR)
    {
        logWarning("signal(SIGTERM, ...) function error");
        exit(EXIT_FAILURE);
    }



    /* Try to create a socket */
    connectionPortNumber = msgMANAGER_PORT_NUMBER;
    connectionSocket = msgSocketCreate(&connectionPortNumber, SOCK_STREAM);
    if (connectionSocket == -1)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }

    /* Accept connection demands on the newly created socket */
    listen(connectionSocket, msgMANAGER_MAX_LENGTH_QUEUE) ;

    /* select() mask initialization */
    FD_ZERO(&readReferenceMask);
    FD_SET(connectionSocket, &readReferenceMask) ;

    /* Connected processes list initialization */
    msgManagerProcessListInit(&procList, &readReferenceMask);

    /* For ever... */
    for(;;)
    {
        int              status ;

        FD_ZERO(&readMask);
        readMask = readReferenceMask;
        
        logInfo("Waiting message...");
        status = select(msgMANAGER_SELECT_WIDTH, &readMask, (fd_set *) NULL,
                        (fd_set *) NULL, (struct timeval *) NULL );

        /* If an error occured in the select() function... */
        if (status == -1)
        {
            /* Raise an error */
            logWarning("ERROR : select() failed - %s\n", strerror(errno));
            
            /* Wait 1 second in order not to loop infinitly (and thus using
             * 100% of the CPU) if its a recurrent error
             */
            sleep(1);
        }

        /* If a new connection demand was received... */
        if (FD_ISSET(connectionSocket , &readMask))
        {
            logInfo("Connection demand received...");

            /* Try to init the new connection */
            if (msgManagerProcessSetConnection(connectionSocket, &procList)
                == FAILURE)
            {
                errCloseStack();
            }
        }
        else /* A message from an already connected process was received... */
        {
            msgPROCESS *currProcess;
            mcsINT32   nbBytesToRead ;

            /* For each already connected process... */
            currProcess = procList.header;
            while (currProcess != NULL)
            {
                /* Store the next process pointer to prevent errors in case of
                 * the current process element destruction
                 */
                msgPROCESS *next = currProcess->next;

                /* If the message come from the current process socket... */
                if (FD_ISSET(currProcess->sd, &readMask))
                {
                    /* If there is some data to be read... */
                    ioctl(currProcess->sd, FIONREAD,
                          (unsigned long *)&nbBytesToRead);
                    if (nbBytesToRead != 0)
                    {
                        /* Try to read the new message */
                        msgMESSAGE_RAW msg;
                        if (msgReceiveFrom(currProcess->sd, &msg, 1000)
                            == FAILURE)
                        {
                            errCloseStack();
                        }
                        else
                        {
                            /* If the new message is a command... */
                            if (msgGetType(&msg) == msgTYPE_COMMAND)
                            {
                                /* If the command is intended to msgManger... */
                                if ((strcmp(msgGetRecipient(&msg), "msgManager")
                                    == 0)
                                    ||
                                    (strcmp (msgGetCommand(&msg), msgPING_CMD)
                                    == 0))
                                {
                                    /* Try to manage the received command */
                                    if (msgManagerHandleCmd(&msg, currProcess,
                                                &procList) == FAILURE)
                                    {
                                        errCloseStack();
                                    }
                                }
                                else /* If the command is not for msgManager */
                                {
                                    /* Try to give it to its process */
                                    if (msgManagerForwardCmd(&msg, currProcess,
                                        &procList) == FAILURE)
                                    {
                                        errCloseStack();
                                    }
                                }
                            }
                            else /* If the message is an answer... */
                            {
                                /* Try to give it to its process */
                                if (msgManagerForwardReply(&msg, currProcess,
                                            &procList) == FAILURE)
                                {
                                    errCloseStack();
                                }
                            }
                        }
                    }
                    else /* If there was nothing to read... */
                    {
                        logWarning("Connection with '%s' process lost",
                                currProcess->name);

                        /* Try to close the connection */
                        if (msgManagerProcessListRemove(&procList, currProcess)
                            == FAILURE)
                        {
                            errCloseStack();
                        }
                    }
                }

                currProcess = next;
            } /* While end */

        }

    } /* For ever end */

    logInfo("msgManager exiting...");
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
