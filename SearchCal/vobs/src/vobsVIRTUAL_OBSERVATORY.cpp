/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsVIRTUAL_OBSERVATORY class definition.
 */

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
    logDebug("vobsVIRTUAL_OBSERVATORY->constructor()");
}


/*
 * Class destructor
 */

/**
 * Delete a virtual observatory object.
 */
vobsVIRTUAL_OBSERVATORY::~vobsVIRTUAL_OBSERVATORY()
{
    logDebug("vobsVIRTUAL_OBSERVATORY->destructor()");
}


/*
 * Public methods
 */

/**
 * Method to initialize the scenario with the given request and optional input list
 * 
 * @param scenario the desired querying scenario
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Init(vobsSCENARIO* scenario,
                                            vobsREQUEST* request,
                                            vobsSTAR_LIST* starList)
{
    logDebug("Init: start");

    // Clear the scenario
    scenario->Clear();

    // Set the catalogList 
    scenario->SetCatalogList(&_catalogList);

    // Launch the stars search
    FAIL_DO(scenario->Init(request, starList), errUserAdd(vobsERR_NO_CDS_RETURN));

    logDebug("Init: done");

    return mcsSUCCESS;
}

/**
 * Start the research according to the constraints found in the given request.
 *
 * @param scenario the desired querying scenario
 * @param starList the resulting list of stars
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Search(vobsSCENARIO *scenario,
                                              vobsSTAR_LIST &starList)
{
    logTest("Search: start");

    // Empty the list
    FAIL(starList.Clear());

    // Launch the stars search
    FAIL_DO(scenario->Execute(starList), errUserAdd(vobsERR_NO_CDS_RETURN));

    logTest("Search: done = Number of stars found : %d", starList.Size());

    return mcsSUCCESS;
}


/*___oOo___*/
