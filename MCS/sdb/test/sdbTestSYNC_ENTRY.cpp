/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sdbTestSYNC_ENTRY.cpp,v 1.1 2007-10-29 12:06:45 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2007/10/26 13:25:26  lafrasse
 * Moved synchronized entry in sdbSYNC_ENTRY, and added timeout-based entry as
 * sdbENTRY.
 *
 * Revision 1.3  2006/12/21 15:03:08  lafrasse
 * Moved from static-based design to instance-based design.
 *
 * Revision 1.2  2006/05/11 13:04:57  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.1  2005/12/20 13:52:34  lafrasse
 * Added preliminary support for INTRA-process action log
 *
 ******************************************************************************/

/**
 * @file
 * sdbSYNC_ENTRY class test program.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: sdbTestSYNC_ENTRY.cpp,v 1.1 2007-10-29 12:06:45 gzins Exp $";

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
    sdbSYNC_ENTRY* entry = (sdbSYNC_ENTRY*) param;

    sleep(1);
    entry->Write("Message 1", mcsFALSE);

    sleep(2);
    entry->Write("Message 2", mcsFALSE);

    sleep(3);
    entry->Write("Message 3", mcsTRUE);

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

    /* Allocate synchronization ressources */
    sdbSYNC_ENTRY entry;
    if (entry.Init() == mcsFAILURE)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    // Thread creation
    thrdTHREAD_STRUCT    myThread;
    myThread.function  = myThreadFunction;
    myThread.parameter = (thrdFCT_ARG)&entry;
    thrdThreadCreate(&myThread);

    mcsSTRING256 message;
    mcsLOGICAL   lastMessage = mcsFALSE;
    do
    {
        if (entry.Wait(message, &lastMessage) == mcsFAILURE)
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

    /* Deallocate synchronization ressources */
    if (entry.Destroy() == mcsFAILURE)
    {
        errCloseStack();
        exit (EXIT_FAILURE);
    }

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
