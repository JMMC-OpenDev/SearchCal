/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrServer.cpp,v 1.14 2010-11-10 15:46:14 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.13  2010/11/09 15:00:59  lafrasse
 * Refactored to cut on EVH dependencies to handle callbacks, preventing compilation under GCC 4.x and newer.
 *
 * Revision 1.12  2007/05/11 15:58:43  gzins
 * Minor documentation fixing
 *
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

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrServer.cpp,v 1.14 2010-11-10 15:46:14 lafrasse Exp $"; 


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
    /*
     * The following instructions have been placed in {} in order to call
     * sclsvrSERVER destructor when application exits.
     */
    {
        /*
         * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! Warning !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         *
         * As of version 3.7.0 and later, sclsvrServer callback mecanism based
         * on MCS module 'evh' is no longuer in use, in order to at last support
         * GCC 4.x and later.
         *
         * The workarround consists in statically parsing the two last CLI args,
         * and run the proper method accordinally.
         */

        /*
         * Init MCS event server, only to handle MCS standard options like '-v',
         * '-h' and so on.
         */
        sclsvrSERVER scalibServer;
        if (scalibServer.Init(argc, argv) == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }

        // Retrieve the COMMAND name to select among GETCAL or GETSTAR mode.
        char* cmdName = argv[argc - 2];
        
        // Get COMMAND argument
        char* cmdArgs = argv[argc - 1];

        miscoDYN_BUF cmdResults;
        mcsCOMPL_STAT cmdStatus = mcsFAILURE;

        // GETCAL mode
        if (strcmp(cmdName, "GETCAL") == 0)
        {
            cmdStatus = scalibServer.GetCal(cmdArgs, cmdResults);
        }
        else
        if (strcmp(cmdName, "GETSTAR") == 0)
        {
            cmdStatus = scalibServer.GetStar(cmdArgs, cmdResults);
        }
        else
        {
            printf("Unknown COMMAND '%s'.\n", cmdName);
        }

        mcsExit();

        if (cmdStatus == mcsFAILURE)
        {
            errCloseStack();
            exit (EXIT_FAILURE);
        }
    }

    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}



