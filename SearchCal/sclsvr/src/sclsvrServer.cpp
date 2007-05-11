/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrServer.cpp,v 1.12 2007-05-11 15:58:43 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.11  2007/05/11 15:41:28  gzins
 * Updated to prevent unreleased semaphores
 *
 * Revision 1.10  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.9  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.8  2005/03/08 16:33:51  gzins
 * Disconnected process from message service before exiting
 *
 * Revision 1.7  2005/02/08 04:52:33  gzins
 * Printed ou error stack when connection to MCS services fails
 *
 * Revision 1.6  2005/02/07 15:01:21  gzins
 * Added CVS log as modification history
 *
 * gzins     23-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * Search Calibrators SERVER
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrServer.cpp,v 1.12 2007-05-11 15:58:43 gzins Exp $"; 


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
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
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    // The following instructions have been placed in {} in order sclsvrSERVER
    // destructor is called when application exits.
    {
        // Init MCS event server
        sclsvrSERVER scalibServer;
        if (scalibServer.Init(argc, argv) == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }

        // Main loop
        while (scalibServer.MainLoop() == mcsFAILURE)
        {
            errDisplayStack();
        }

        // Close MCS services
        scalibServer.Disconnect();
        mcsExit();
    }

    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
