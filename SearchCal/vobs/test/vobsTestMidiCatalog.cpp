/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program for vobsCATALOG_MIDI class.
 *
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestMidiCatalog.cpp,v 1.10 2011-02-25 15:58:58 lafrasse Exp $"; 


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
#include "evh.h"


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
        
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout Log level
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting...");
    
    // Set a task
    evhTASK task;

    // Parse command line
    if (task.Init(argc, argv) == mcsFAILURE)
    {
        exit(EXIT_FAILURE);
    }

    // Set request
    vobsREQUEST request;
    request.SetObjectName("ETA TAU");
    request.SetObjectRa("00 09 24.64");
    request.SetObjectDec("-45 44 50.7");
    request.SetObjectMag(2.50);
    request.SetSearchBand("N");
    request.SetSearchArea(15.0, 5.0);
    request.SetMinMagRange(0.05);
    request.SetMaxMagRange(0.2);
    request.Display();

    // Create a star list
    vobsSTAR_LIST starList;
    
    // Create a MIDI catalog
    vobsCATALOG_MIDI midiCatalog;
    
    // Search for star list from MIDI catalog which verifies user constraints
    midiCatalog.Search(request, starList);
    
    // Display star list size
    printf("Number of selected stars = %d\n", starList.Size());
   
    // Display star list
    starList.Display();

    
    // Logs errors and resets the global error structure
    errCloseStack();
    
    logInfo("Exiting ...");
    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
