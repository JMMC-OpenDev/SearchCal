/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestVirtualObsV.C,v 1.1 2004-08-25 13:46:52 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    25-Aug-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestVirtualObsV.C,v 1.1 2004-08-25 13:46:52 scetre Exp $"; 
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


/*
 * Local Headers 
 */
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsREQUEST.h"
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"

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


    logSetStdoutLogLevel(logEXTDBG);
    
    logInfo("Starting ...");
    vobsREQUEST request;

    // Affect the kind of request
    if (request.SetKindOfRequest(1)==FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE); 
    }

    if ((request.SetConstraint(STAR_NAME_ID,"ETA TAU") == SUCCESS) &&
        (request.SetConstraint(STAR_WLEN_ID,"0.65") == SUCCESS) &&
        (request.SetConstraint(STAR_MAGNITUDE_ID,"2.87") == SUCCESS) &&
        (request.SetConstraint(MAGNITUDE_RANGE_ID,"1..5") == SUCCESS) &&
        (request.SetConstraint(SEARCH_BOX_RA_ID,"03+47+29.08") == SUCCESS) &&
        (request.SetConstraint(SEARCH_BOX_DEC_ID,"+24+06+18.5") == SUCCESS) &&
        (request.SetConstraint(STAR_EXPECTED_VIS_ID,"0.349") == SUCCESS) &&
        (request.SetConstraint(STAR_MAX_ERR_VIS_ID,"0.035") == SUCCESS) &&
        (request.SetConstraint(OBSERVED_BAND_ID,"V") == SUCCESS ))
    {
        // Check if the request is correctly build
        if (request.Check()==mcsTRUE)
        {
            // Display of the request correctly build
            request.Display();
        }
        else
        {
            errDisplayStack();
            errCloseStack();
            exit(EXIT_FAILURE);
        }
    }
    else 
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    errDisplayStack();

    vobsSTAR_LIST starList;

    vobsVIRTUAL_OBSERVATORY vobs;

    if (vobs.Research(request, starList)==FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    
    errDisplayStack();
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);

}


/*___oOo___*/
