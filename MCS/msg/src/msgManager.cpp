/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: msgManager.cpp,v 1.6 2006-05-11 13:04:56 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2005/02/04 15:57:06  lafrasse
 * Massive documentation review an refinment (also added automatic CVS log inclusion in every files)
 *
 * Revision 1.4  2005/01/24 15:02:47  gzins
 * Added CVS logs as modification history
 *
 * gzins     07-Jan-2005  Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE 
 * lafrasse  15-Dec-2004  Re-added Doxygen documentation from the npw removed
 *                        msgManager.c
 * gzins     06-Dec-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * \e \<msgManager\> - MCS inter-process communication server.
 *
 * \b Synopsis:\n
 * \e \<msgManager\>
 *
 * \b Details:\n
 * \e \<msgManager\> is the communication server doing message forwarding
 * between processes. Each process connected to this server can send messages to
 * (and receive messages from) the other connected processes.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: msgManager.cpp,v 1.6 2006-05-11 13:04:56 mella Exp $";

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
