/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsTestMidiCatalog.cpp,v 1.2 2005-02-11 16:41:40 gluck Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/02/08 06:38:44  gzins
 * Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program for vobsCATALOG_MIDI class.
 *
 * \synopsis
 * vobsTestMidiCatalog [\e \<param1\> ... \e \<paramN\>] 
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

static char *rcsId="@(#) $Id: vobsTestMidiCatalog.cpp,v 1.2 2005-02-11 16:41:40 gluck Exp $"; 
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
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout Log level
    logSetStdoutLogLevel(logDEBUG);

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
    request.SetMinDeltaMag(0.05);
    request.SetMaxDeltaMag(0.2);
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
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
