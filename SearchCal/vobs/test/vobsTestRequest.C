/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsTestRequest.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    26-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsTestRequest.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"; 
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

    logSetStdoutLogStateLevel(logEXTDBG);

    logInfo("Starting ...");

    vobsREQUEST request;
    logTest("request brefore building : ");
    // Display the request to see if the element of the class are correctly
    // initialized
    request.Display();
 
    // Affect the kind of request
    if (request.SetKindOfRequest(1)==FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE); 
    }
    
    // Get the kind of request
    int kindOfReq=0;
    if (request.GetKindOfRequest(kindOfReq)==FAILURE)
    {
        errDisplayStack();
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    cout << "kind of request : " << kindOfReq << endl;
    
    // Set element of the class
    if (request.SetConstraint(STAR_NAME_ID,"ETA TAU")&&
        request.SetConstraint(STAR_WLEN_ID,"2.20")&&
        request.SetConstraint(STAR_MAGNITUDE_ID,"2.50")&&
        request.SetConstraint(MAGNITUDE_RANGE_ID,"-1..5")&&
        request.SetConstraint(SEARCH_BOX_RA_ID,"02+12+52.3")&&
        request.SetConstraint(SEARCH_BOX_DEC_ID,"+06+04+36.2")&&
        request.SetConstraint(STAR_EXPECTED_VIS_ID,"2")&&
        request.SetConstraint(STAR_MAX_ERR_VIS_ID,"0.00032")&&
        request.SetConstraint(OBSERVED_BAND_ID,"K"))
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
    
       
    
    logInfo("Exiting ...");
    exit(EXIT_SUCCESS);    
}


/*___oOo___*/
