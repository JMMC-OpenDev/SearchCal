/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test of the catalog List object.
 */



/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

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
#include "vobs.h"
#include "vobsPrivate.h"


/*
 * Local Variables
 */



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
        // Error handling if necessary

        // Exit from the application with FAILURE
        exit(EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting ...");

    vobsSTAR star;

    logTest("Creation of the catalog list...");
    vobsCATALOG_LIST catalogList;
    logTest("\tDone.");
    vobsCATALOG *catalog;
    logTest("The catalog list is a static class. At its creation, all existing catalog are put in it. Normally, it is possible to retreive a specific catalog");
    logTest("Get catalog I/280...");
    catalog = catalogList.Get(vobsCATALOG_ASCC_ID);
    logTest("\tDone.");
    logTest("in the catalog list is present the catalog %s",
            catalog->GetName());

    logInfo("Exiting ...");
    // Close MCS services
    mcsExit();

    // Exit from the application with SUCCESS
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
