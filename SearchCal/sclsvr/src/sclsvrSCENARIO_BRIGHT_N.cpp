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
sclsvrSCENARIO_BRIGHT_N::sclsvrSCENARIO_BRIGHT_N(sdbENTRY* progress):
    vobsSCENARIO(progress)
{
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

/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_N::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_N::Init()");

    // Clear the scenario
    Clear();
    _request.Copy(*request);

    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();

    if (AddEntry(vobsCATALOG_MIDI_ID, &_request, NULL, &_starListS, vobsCOPY)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
