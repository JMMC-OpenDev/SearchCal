/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestRequest.cpp,v 1.2 2005-02-04 08:06:44 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    26-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestRequest.cpp,v 1.2 2005-02-04 08:06:44 scetre Exp $"; 
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
#include "vobsREQUEST.h"
#include "vobsPrivate.h"


int main(int argc, char *argv[])
{
    mcsInit(argv[0]);


    logInfo("Starting ...");

    vobsREQUEST request;
    logTest("request brefore building : ");
    // Display the request to see if the element of the class are correctly
    // initialized
 
    // Set element of the class
    request.SetConstraint(STAR_NAME_ID,"ETA TAU");
    request.SetConstraint(RA_ID,"02+12+52.3");
    request.SetConstraint(DEC_ID,"+06+04+36.2");
    request.SetConstraint(STAR_WLEN_ID,"2.20");
    request.SetConstraint(STAR_MAGNITUDE_ID,"2.50");
    request.SetConstraint(MIN_MAGNITUDE_RANGE_ID,"1");
    request.SetConstraint(MAX_MAGNITUDE_RANGE_ID,"5");
    request.SetConstraint(SEARCH_BOX_RA_ID,"1800");
    request.SetConstraint(SEARCH_BOX_DEC_ID,"300");
    request.SetConstraint(STAR_EXPECTED_VIS_ID,"2");
    request.SetConstraint(STAR_MAX_ERR_VIS_ID,"0.00032");
    request.SetConstraint(OBSERVED_BAND_ID,"K");
    request.SetConstraint(BASEMIN_ID,"46");
    request.SetConstraint(BASEMAX_ID,"100");
    
        
    
    
    errDisplayStack();
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);    
}


/*___oOo___*/
