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
 * Method to initialize the scenario with the given request and optional input list
 *
 * @param scenario the desired querying scenario
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Init(vobsSCENARIO* scenario,
                                            vobsREQUEST* request,
                                            vobsSTAR_LIST* starList)
{
    // Clear the scenario
    scenario->Clear();

    // Set the catalogList
    scenario->SetCatalogList(&_catalogList);

    // Launch the stars search
    FAIL_DO(scenario->Init(_ctx, request, starList), 
            errUserAdd(vobsERR_NO_CDS_RETURN));

    return mcsSUCCESS;
}

/**
 * Start the research according to the constraints found in the given request.
 *
 * @param scenario the desired querying scenario
 * @param starList the resulting list of stars
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsVIRTUAL_OBSERVATORY::Search(vobsSCENARIO *scenario,
                                              vobsSTAR_LIST &starList)
{
    logTest("Search: start");

    // Empty the list
    starList.Clear();

    // Launch the stars search
    FAIL_DO(scenario->Execute(_ctx, starList), 
            errUserAdd(vobsERR_NO_CDS_RETURN));

    logTest("Search: done [%d stars]", starList.Size());

    return mcsSUCCESS;
}


/*___oOo___*/
