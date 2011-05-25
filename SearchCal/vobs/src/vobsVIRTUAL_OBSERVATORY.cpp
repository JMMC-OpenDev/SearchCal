/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsVIRTUAL_OBSERVATORY class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsVIRTUAL_OBSERVATORY.cpp,v 1.37 2006-04-07 08:23:00 gzins Exp $";


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
#include "vobsVIRTUAL_OBSERVATORY.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"


/*
 * Class constructor
 */
/**
 * Build a virtual observatory object.
 */
vobsVIRTUAL_OBSERVATORY::vobsVIRTUAL_OBSERVATORY()
{
}


/*
 * Class destructor
 */
/**
 * Delete a virtual observatory object.
 */
vobsVIRTUAL_OBSERVATORY::~vobsVIRTUAL_OBSERVATORY()
{
}


/*
 * Public methods
 */
/**
 * Start the research according to the constraints found in the given request.
 *
 * @param scenario the desired querying scenario
 * @param request the user constarint the foun d stars should conform to
 * @param starList the resulting list of stars
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Search(vobsSCENARIO   *scenario,
                                              vobsREQUEST    &request,
                                              vobsSTAR_LIST  &starList)
{
    logTrace("vobsVIRTUAL_OBSERVATORY::Search()");

    // Set the catalogList 
    scenario->SetCatalogList(&_catalogList);

    // Empty the list
    if (starList.Clear() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Launch the stars search
    if (scenario->Execute(starList) == mcsFAILURE)
    {
        errUserAdd(vobsERR_NO_CDS_RETURN);
        return mcsFAILURE;
    }
    
    //starList.Display();
    logTest("Number of stars found : %d", starList.Size());

    return mcsSUCCESS;
}


/*___oOo___*/
