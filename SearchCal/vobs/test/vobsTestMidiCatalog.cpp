/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestMidiCatalog.cpp,v 1.7 2005-11-16 10:45:14 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.6  2005/02/16 15:30:32  gzins
 * Fixed wrong mcsmcs prefix
 *
 * Revision 1.5  2005/02/15 15:27:41  gzins
 * Changed min/maxRangeMag to min/maxMagRange
 *
 * Revision 1.4  2005/02/15 15:26:06  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.3  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * Revision 1.2  2005/02/11 16:41:40  gluck
 * Updated to be fit with new vobsREQUEST class
 *
 * Revision 1.1  2005/02/08 06:38:44  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program for vobsCATALOG_MIDI class.
 *
 */

static char *rcsId="@(#) $Id: vobsTestMidiCatalog.cpp,v 1.7 2005-11-16 10:45:14 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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
    request.SetDeltaRa(15.0);
    request.SetDeltaDec(5.0);
    request.SetMinMagRange(0.05);
    request.SetMaxMagRange(0.2);
    request.SetMaxNbOfSelectedObjects(6);
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
