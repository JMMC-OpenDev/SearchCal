/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestVirtualobs.C,v 1.6 2004-10-18 11:39:31 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    30-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestVirtualobs.C,v 1.6 2004-10-18 11:39:31 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
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
#include "vobsSTAR.h"
#include "vobsSTAR_LIST.h"
#include "vobsREQUEST.h"
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"


/*
 * Local Variables
 */

 
/* 
 * Signal catching functions  
 */


/*
 * Local Functions
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
        (request.SetConstraint(RA_ID,"03+47+29.08") == SUCCESS) &&
        (request.SetConstraint(DEC_ID,"+24+06+18.5") == SUCCESS) &&
        (request.SetConstraint(STAR_WLEN_ID,"0.65") == SUCCESS) &&
        (request.SetConstraint(STAR_MAGNITUDE_ID,"2.96") == SUCCESS) &&
        (request.SetConstraint(MAGNITUDE_RANGE_ID,"-1..5") == SUCCESS) &&
        (request.SetConstraint(SEARCH_BOX_RA_ID,"1800") == SUCCESS) &&
        (request.SetConstraint(SEARCH_BOX_DEC_ID,"300") == SUCCESS) &&
        (request.SetConstraint(STAR_EXPECTED_VIS_ID,"0.922") == SUCCESS) &&
        (request.SetConstraint(STAR_MAX_ERR_VIS_ID,"0.09") == SUCCESS) &&
        (request.SetConstraint(OBSERVED_BAND_ID,"K") == SUCCESS ))
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
