/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdbTestSynckedEntry.cpp,v 1.2 2006-05-11 13:04:57 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/12/20 13:52:34  lafrasse
 * Added preliminary support for INTRA-process action log
 *
 ******************************************************************************/

/**
 * @file
 * sdbEntry class test program.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: sdbTestSynckedEntry.cpp,v 1.2 2006-05-11 13:04:57 mella Exp $";

/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <unistd.h>


/**
 * @namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "thrd.h"


/*
 * Local Headers 
 */
#include "sdb.h"
#include "sdbPrivate.h"


/*
 * Local functions
 */
thrdFCT_RET myThreadFunction(thrdFCT_ARG param)
{
    sleep(1);
    sdbWriteAction("Message 1", mcsFALSE);

    sleep(2);
    sdbWriteAction("Message 2", mcsFALSE);

    sleep(3);
    sdbWriteAction("Message 3", mcsTRUE);

    return NULL;
}

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }
    
    logSetStdoutLogLevel(logINFO);

    sdbInitAction();

    // Thread creation
    thrdTHREAD           myThread;
    myThread.function  = myThreadFunction;
    myThread.parameter = NULL;
    thrdThreadCreate(&myThread);

    mcsSTRING256 message;
    mcsLOGICAL   lastMessage = mcsFALSE;
    do
    {
        if (sdbWaitAction(message, &lastMessage) == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }

        if (lastMessage == mcsFALSE)
        {
            cout << "Received message : '" << message <<"'." << endl;
        }
        else
        {
            cout << "Received LAST message : '" << message <<"'." << endl;
        }
    }
    while (lastMessage == mcsFALSE);

    /* Wait for the thread end */
    thrdThreadWait(&myThread);

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
