/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSEARCH_CALIBRATORS.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    15-Sep-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrSEARCH_CALIBRATORS class definition.
 */

static char *rcsId="@(#) $Id: sclsvrSEARCH_CALIBRATORS.C,v 1.1 2004-11-25 13:12:55 scetre Exp $"; 
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
#include "sclsvr.h"
#include "sclsvrPrivate.h"


/*
 * Class constructor
 */
sclsvrSEARCH_CALIBRATORS::sclsvrSEARCH_CALIBRATORS()
{
}

/*
 * Class destructor
 */
sclsvrSEARCH_CALIBRATORS::~sclsvrSEARCH_CALIBRATORS()
{
}


/*
 * Public methods
 */
/**
 * Decode a command.
 *
 * Decode a command in order to build the request
 * 
 * \param argc count of the arguments supplied to the method
 * \param argv array of pointers to the strings which are those arguments
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSEARCH_CALIBRATORS::DecodeCommand(int argc, char *argv[])
{
    logExtDbg("sclsvrSEARCH_CALIBRATORS::DecodeCommand()");
    
    sclsvrSERVER sclsvrServer;
    // Parse input parameter
    if (sclsvrServer.ParseOptions(argc, argv) == FAILURE)
    {
        exit (EXIT_FAILURE);
    }
    sclsvrServer.Display();

    if (request.SetKindOfRequest(1)==FAILURE)
    {
        return FAILURE; 
    }
    if (request.SetConstraint(STAR_NAME_ID,sclsvrServer.GetObjectName()) 
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(RA_ID,sclsvrServer.GetRa()) 
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(DEC_ID,sclsvrServer.GetDec()) 
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(STAR_WLEN_ID,sclsvrServer.GetLambda())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(STAR_MAGNITUDE_ID,sclsvrServer.GetMag())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(MAGNITUDE_RANGE_ID,sclsvrServer.GetMagrange())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(SEARCH_BOX_RA_ID,sclsvrServer.GetDiffRa())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(SEARCH_BOX_DEC_ID,sclsvrServer.GetDiffDec())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(STAR_EXPECTED_VIS_ID,sclsvrServer.GetVis())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(STAR_MAX_ERR_VIS_ID,sclsvrServer.GetVisErr())
        == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(OBSERVED_BAND_ID,sclsvrServer.GetBand())
        ==  FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(BASEMIN_ID,sclsvrServer.GetBaseMin())
        ==  FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(BASEMAX_ID,sclsvrServer.GetBaseMax())
        ==  FAILURE)
    {
        return FAILURE;
    }
    //request.Display();

    return SUCCESS;
    
}

/**
 * Run the search Calibrators.
 *
 * According to the request, the method use the virtua observatory object to
 * begin a research, get the resuting list of star and complete it to have a
 * final list of potential calibrators.
 * 
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSEARCH_CALIBRATORS::Run()
{
    logExtDbg("sclsvrSEARCH_CALIBRATORS::Run()");
    // start the research in the virtual observatory
    if (virtualObservatory.Search(request, starList)==FAILURE)
    {
        return FAILURE;
    }
    // get the resulting star list and create a calibrator list
    if (calibratorList.Copy(starList) == FAILURE)
    {
        return FAILURE;
    }
    // complete the calibrators list
    if (calibratorList.Complete(request) == FAILURE)
    {
        return FAILURE;
    }
    calibratorList.Display();
    return SUCCESS;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
