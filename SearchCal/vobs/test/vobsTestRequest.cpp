/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestRequest.cpp,v 1.6 2005-02-15 15:26:06 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * vobsTestRequest [\e \<param1\> ... \e \<paramN\>] 
 *                     [\e \<option1\> ... \e \<optionN\>] 
 *
 * \param param1 : description of parameter 1, if it exists
 * \param paramN : description of parameter N, if it exists
 *
 * \n
 * \opt
 * \optname option1 : description of option 1, if it exists
 * \optname optionN : description of option N, if it exists
 * 
 * \n
 * \details
 * OPTIONAL detailed description of the c main file follows here.
 * 
 * \usedfiles
 * OPTIONAL. If files are used, for each one, name, and usage description.
 * \filename fileName1 :  usage description of fileName1
 * \filename fileName2 :  usage description of fileName2
 *
 * \n
 * \env
 * OPTIONAL. If needed, environmental variables accessed by the program. For
 * each variable, name, and usage description, as below.
 * \envvar envVar1 :  usage description of envVar1
 * \envvar envVar2 :  usage description of envVar2
 * 
 * \n
 * \warning OPTIONAL. Warning if any (software requirements, ...)
 *
 * \n
 * \ex
 * OPTIONAL. Command example if needed
 * \n Brief example description.
 * \code
 * Insert your command example here
 * \endcode
 *
 * \sa OPTIONAL. See also section, in which you can refer other documented
 * entities. Doxygen will create the link automatically.
 * \sa <entity to refer>
 * 
 * \bug OPTIONAL. Known bugs list if it exists.
 * \bug Bug 1 : bug 1 description
 *
 * \todo OPTIONAL. Things to forsee list, if needed. 
 * \todo Action 1 : action 1 description
 * 
 */

static char *rcsId="@(#) $Id: vobsTestRequest.cpp,v 1.6 2005-02-15 15:26:06 gzins Exp $"; 
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
    logSetStdoutLogLevel(logDEBUG);

    logInfo("Starting...");
    
    // Set a task
    evhTASK task;

    // Parse command line
    if (task.Init(argc, argv) == mcsmcsFAILURE)
    {
        exit(EXIT_FAILURE);
    }

    vobsREQUEST request;
    
    // Display initial request
    logDebug("-------------------------------------------------");
    logDebug("-> Initial request() ...");
    request.Display();
    
    // Set request
    logDebug("-------------------------------------------------");
    logDebug("-> Test of set() methods ...");
    request.SetObjectName("ETA TAU");
    request.SetObjectRa("12 04 36.2");
    request.SetObjectDec("-30 91 21.4");
    request.SetObjectMag(1.3);
    request.SetSearchBand("M");
    request.SetDeltaRa(12);
    request.SetDeltaDec(8);
    request.SetMinRangeMag(0.4);
    request.SetMaxRangeMag(0.2);
    request.SetMaxNbOfSelectedObjects(14);

    // Display affected request
    logDebug("-------------------------------------------------");
    logDebug("-> Affected request() ...");
    request.Display();

    // Set request
    logDebug("-------------------------------------------------");
    logDebug("-> Test of get() methods ...");
    logDebug("Get object name = %s", request.GetObjectName());
    logDebug("Get ra = %f", request.GetObjectRa());
    logDebug("Get dec = %f", request.GetObjectDec());
    logDebug("Get Mag = %f", request.GetObjectMag());
    logDebug("Get search band = %c", request.GetSearchBand());
    logDebug("Get delta ra = %f", request.GetDeltaRa());
    logDebug("Get delta dec = %f", request.GetDeltaDec());
    logDebug("Get min delta mag = %f", request.GetMinRangeMag());
    logDebug("Get max delta mag = %f", request.GetMaxRangeMag());
    logDebug("Get max nb of selected object = %i", 
             request.GetMaxNbOfSelectedObjects());

    // Logs errors and resets the global error structure
    errCloseStack();
    
    logInfo("Exiting ...");
    
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
