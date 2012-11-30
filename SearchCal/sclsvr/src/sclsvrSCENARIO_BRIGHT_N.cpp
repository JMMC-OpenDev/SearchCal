/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_N class.
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
#include "sclsvrSCENARIO_BRIGHT_N.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_N::sclsvrSCENARIO_BRIGHT_N(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
    // disable duplicates detection because MIDI catalog does not contain duplicates:
    SetRemoveDuplicates(false);
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_N::~sclsvrSCENARIO_BRIGHT_N()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "BRIGHT_N"
 */
const char* sclsvrSCENARIO_BRIGHT_N::GetScenarioName()
{
    return "BRIGHT_N";
}

/**
 * Initialize the BRIGHT N scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_N::Init(vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    logTrace("sclsvrSCENARIO_BRIGHT_N::Init()");

    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // PRIMARY REQUEST

    // No duplicates in the MIDI catalog:
    FAIL(AddEntry(vobsCATALOG_MIDI_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDec));

    return mcsSUCCESS;
}


/*___oOo___*/
