/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * \e \<msgManager\> - MCS inter-process communication server.
 *
 * \b Synopsis:
 * \e \<msgManager\>
 *
 * \b Details:
 * \e \<msgManager\> is the communication server doing message forwarding
 * between processes. Each process connected to this server can send messages to
 * (and receive messages from) the other connected processes.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgManager.cpp,v 1.7 2007-02-22 12:27:59 gzins Exp $";

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "msgMANAGER.h"
#include "msgPrivate.h"

/*
 * Local Variables
 */
static msgMANAGER *msgManager=NULL;

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

void msgSignalHandler (int signalNumber)
{
    logInfo("Received %d system signal...", signalNumber);
    if (signalNumber == SIGPIPE)
    {
        return;
    }
    logInfo("%s program aborted.", mcsGetProcName());
    delete (msgManager);
    exit (EXIT_SUCCESS);
}

/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Message manager instance
    msgManager = new msgMANAGER;
    
    /* Init system signal trapping */
    if (signal(SIGINT, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGINT, ...) function error");
        exit(EXIT_FAILURE);
    }
    if (signal (SIGTERM, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        exit(EXIT_FAILURE);
    }
    if (signal (SIGPIPE, msgSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        exit(EXIT_FAILURE);
    }

    // Initialization
    if (msgManager->Init(argc, argv) == mcsFAILURE)
    {
        // Close error stack
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    // Enter in main loop
    if (msgManager->MainLoop() == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }

    // Exit from the application with mcsSUCCESS
    delete (msgManager);
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
