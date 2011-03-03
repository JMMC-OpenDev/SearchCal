/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestRequest.cpp,v 1.13 2011-03-03 13:09:43 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2011/02/25 15:58:58  lafrasse
 * Removed date and filelines from test outputs.
 *
 * Revision 1.11  2006/03/03 15:17:43  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.10  2005/11/23 17:30:20  lafrasse
 * Added circular search box geometry support and normalized area size methods
 *
 * Revision 1.9  2005/11/16 10:45:14  scetre
 * Updated vobs test
 *
 * Revision 1.8  2005/02/16 15:30:32  gzins
 * Fixed wrong mcsmcs prefix
 *
 * Revision 1.7  2005/02/15 15:27:41  gzins
 * Changed min/maxRangeMag to min/maxMagRange
 *
 * Revision 1.6  2005/02/15 15:26:06  gzins
 * Changed SUCCESS/FAILURE to mcsSUCCESS/mcsFAILURE
 *
 * Revision 1.5  2005/02/13 08:54:00  gzins
 * Updated after vobs classes changes
 * Added CVS log as modifification history
 *
 * Revision 1.4  2005/02/07 17:43:06  gluck
 * Clean up comment code
 *
 * Revision 1.3  2005/02/07 17:28:34  gluck
 * New vobsREQUEST class implementation tests
 *
 ******************************************************************************/

/**
 * \file
 * Test program for vobsREQUEST class.
 *
 * \synopsis
 * vobsTestRequest [] 
 *
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: vobsTestRequest.cpp,v 1.13 2011-03-03 13:09:43 lafrasse Exp $"; 


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

    vobsREQUEST request;
    
    // Display initial request
    logTest("-------------------------------------------------");
    logTest("-> Initial request() ...");
    request.Display();
    
    // Set request
    logTest("-------------------------------------------------");
    logTest("-> Test of set() methods ...");
    request.SetObjectName("ETA TAU");
    request.SetObjectRa("12 04 36.2");
    request.SetObjectDec("-30 91 21.4");
    request.SetObjectMag(1.3);
    request.SetSearchBand("M");
    request.SetSearchArea(12, 8);
    request.SetMinMagRange(0.4);
    request.SetMaxMagRange(0.2);

    // Display affected request
    logTest("-------------------------------------------------");
    logTest("-> Affected request() ...");
    request.Display();

    // Set request
    logTest("-------------------------------------------------");
    logTest("-> Test of get() methods ...");
    logTest("Get object name = %s", request.GetObjectName());
    logTest("Get ra = %f", request.GetObjectRa());
    logTest("Get dec = %f", request.GetObjectDec());
    logTest("Get Mag = %f", request.GetObjectMag());
    logTest("Get search band = %s", request.GetSearchBand());
    char* type = NULL;
    switch(request.GetSearchAreaGeometry())
    {
        case vobsBOX:
            type = "RECTANGULAR";
            break;

        case vobsCIRCLE:
            type = "CIRCULAR";
            break;

        default:
            type = "UNKNOWN";
            break;
    }
    logTest("Get search area geometry: %s", type);
    mcsDOUBLE deltaRa, deltaDec, radius;
    switch(request.GetSearchAreaGeometry())
    {
        case vobsBOX:
            request.GetSearchArea(deltaRa, deltaDec);
            logTest("Get search area size: ra = %f; dec = %f", deltaRa, deltaDec);
            break;

        case vobsCIRCLE:
            request.GetSearchArea(radius);
            logTest("Get search area size: radius = %f", radius);
            break;

        default:
            logTest("Get search area size: UNKNOWN");
            break;
    }
    logTest("Get min delta mag = %f", request.GetMinMagRange());
    logTest("Get max delta mag = %f", request.GetMaxMagRange());

    // Logs errors and resets the global error structure
    errCloseStack();
    
    logInfo("Exiting ...");
    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
