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
#include <malloc.h>
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "timlog.h"


/*
 * Local Headers 
 */
#include "sclsvr.h"
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    // Turn off mmap usage (slower and do not release memory efficiently)
    mallopt (M_MMAP_MAX, 0);

    mcsCOMPL_STAT cmdStatus = mcsFAILURE;
    
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
        if (scalibServer.Init(argc, argv) == mcsSUCCESS)
        {
            // initialize property meta data:
            sclsvrInit();

            // Retrieve the COMMAND name to select among GETCAL or GETSTAR mode.
            char* cmdName = argv[argc - 2];

            // Get COMMAND argument
            char* cmdArgs = argv[argc - 1];

            // GETCAL mode
            if (strcmp(cmdName, "GETCAL") == 0)
            {
                cmdStatus = scalibServer.GetCal(cmdArgs);
            }
            else
            if (strcmp(cmdName, "GETSTAR") == 0)
            {
                cmdStatus = scalibServer.GetStar(cmdArgs);
            }
            else
            {
                printf("Unknown COMMAND '%s'.\n", cmdName);
            }
        }
    } // free sclsvrSERVER

    if (cmdStatus == mcsFAILURE)
    {
        errCloseStack();
    }

    // free property meta data:
    sclsvrExit();

    // free the timlog table:
    timlogClear();

    // Stop err module:
    errExit();

    mcsExit();

    // Exit from the application:
    exit((cmdStatus == mcsSUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE);
}



