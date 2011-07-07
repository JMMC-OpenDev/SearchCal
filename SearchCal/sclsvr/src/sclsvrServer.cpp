/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Search Calibrators SERVER
 */



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



