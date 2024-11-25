/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_JSDC_FAINT class.
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
#include "sclsvrSCENARIO_JSDC_FAINT.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_JSDC_FAINT::sclsvrSCENARIO_JSDC_FAINT(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
    // Save the xml output (last chunk)
    // _saveSearchXml = mcsTRUE;

    // Load and Save intermediate results
    _loadSearchList = true;
    _saveSearchList = true;
    // disable duplicates removal in latest SIMBAD x ASCC (2017.4):
    _removeDuplicates = false;
}

/**
 * Class destructor
 */
sclsvrSCENARIO_JSDC_FAINT::~sclsvrSCENARIO_JSDC_FAINT()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC_FAINT"
 */
const char* sclsvrSCENARIO_JSDC_FAINT::GetScenarioName() const
{
    return "JSDC_FAINT";
}

/**
 * Initialize the JSDC FAINT scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC_FAINT::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    ////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST on LOCAL CATALOG
    ////////////////////////////////////////////////////////////////////////

    // Get only RA/Dec (J2000 - epoch 2000) + pmRa/Dec + optional SpType/ObjType (SIMBAD)
    FAIL(AddEntry(vobsCATALOG_JSDC_FAINT_LOCAL_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDecJSDC));

    // Merge with I/280 to get all catalog properties
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecJSDC));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // 2MASS
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2MASS));

    // I/355/gaiadr3
    FAIL(AddEntry(vobsCATALOG_GAIA_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaia));
    // I/355/paramp
    FAIL(AddEntry(vobsCATALOG_GAIA_AP_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaiaAP));

    // II/328/allwise aka WISE (LMN)
    FAIL(AddEntry(vobsCATALOG_WISE_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecWise));

    // B/sb9 - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // B/wds/wds - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // II/297/irc aka AKARI (out of scope: FAINT)

    // II/361 - MDFC (out of scope: FAINT)

    return mcsSUCCESS;
}


/*___oOo___*/
