/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrSEARCH_CALIBRATORS.cpp,v 1.2 2005-01-24 13:40:27 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrSEARCH_CALIBRATORS.cpp,v 1.2 2005-01-24 13:40:27 scetre Exp $"; 
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSEARCH_CALIBRATORS::DecodeCommand(int argc, char *argv[])
{
    logExtDbg("sclsvrSEARCH_CALIBRATORS::DecodeCommand()");
    
    sclsvrSERVER sclsvrServer;
    // Parse input parameter
    if (sclsvrServer.ParseOptions(argc, argv) == mcsFAILURE)
    {
        exit (EXIT_mcsFAILURE);
    }
    sclsvrServer.Display();

    if (request.SetKindOfRequest(1)==mcsFAILURE)
    {
        return mcsFAILURE; 
    }
    if (request.SetConstraint(STAR_NAME_ID,sclsvrServer.GetObjectName()) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(RA_ID,sclsvrServer.GetRa()) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(DEC_ID,sclsvrServer.GetDec()) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(STAR_WLEN_ID,sclsvrServer.GetLambda())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(STAR_MAGNITUDE_ID,sclsvrServer.GetMag())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(MAGNITUDE_RANGE_ID,sclsvrServer.GetMagrange())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(SEARCH_BOX_RA_ID,sclsvrServer.GetDiffRa())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(SEARCH_BOX_DEC_ID,sclsvrServer.GetDiffDec())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(STAR_EXPECTED_VIS_ID,sclsvrServer.GetVis())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(STAR_MAX_ERR_VIS_ID,sclsvrServer.GetVisErr())
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(OBSERVED_BAND_ID,sclsvrServer.GetBand())
        ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(BASEMIN_ID,sclsvrServer.GetBaseMin())
        ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (request.SetConstraint(BASEMAX_ID,sclsvrServer.GetBaseMax())
        ==  mcsFAILURE)
    {
        return mcsFAILURE;
    }
    //request.Display();

    return mcsSUCCESS;
    
}

/**
 * Run the search Calibrators.
 *
 * According to the request, the method use the virtua observatory object to
 * begin a research, get the resuting list of star and complete it to have a
 * final list of potential calibrators.
 * 
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSEARCH_CALIBRATORS::Run()
{
    logExtDbg("sclsvrSEARCH_CALIBRATORS::Run()");
    // start the research in the virtual observatory
    if (virtualObservatory.Search(request, starList)==mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // get the resulting star list and create a calibrator list
    if (calibratorList.Copy(starList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // complete the calibrators list
    if (calibratorList.Complete(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    calibratorList.Display();
    return mcsSUCCESS;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
