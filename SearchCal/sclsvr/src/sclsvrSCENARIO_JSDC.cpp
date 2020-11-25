/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_JSDC class.
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
#include "sclsvrSCENARIO_JSDC.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_JSDC::sclsvrSCENARIO_JSDC(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
    // Load and Save intermediate results
    _loadSearchList = true;
    _saveSearchList = true;
    // disable duplicates removal in latest SIMBAD x ASCC (2017.4):
    _removeDuplicates = false;
}

/**
 * Class destructor
 */
sclsvrSCENARIO_JSDC::~sclsvrSCENARIO_JSDC()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC"
 */
const char* sclsvrSCENARIO_JSDC::GetScenarioName() const
{
    return "JSDC_BRIGHT";
}

/**
 * Initialize the JSDC scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
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

    // Get only RA/Dec (J2000 - epoch 2000) + pmRa/Dec + optional SpType/ObjType
    FAIL(AddEntry(vobsCATALOG_JSDC_LOCAL_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDecJSDC));

    // Merge with I/280 to get all catalog properties
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecJSDC));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // I/311 - Hipparcos, the New Reduction (van Leeuwen, 2007)
    // to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // HIP1 - V / B / Ic (2013-04-18)
    FAIL(AddEntry(vobsCATALOG_HIP1_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // I/345/gaia2
    FAIL(AddEntry(vobsCATALOG_GAIA_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaia));
    // I/347/gaia2dis
    FAIL(AddEntry(vobsCATALOG_GAIA_DIST_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaiaDist));

    // 2MASS
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2MASS));

    // II/328/allwise aka WISE (LMN)
    FAIL(AddEntry(vobsCATALOG_WISE_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMidIR));

    // II/7A - UBVRIJKLMNH Photoelectric Catalogue
    FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMagV));

    // I/196
    FAIL(AddEntry(vobsCATALOG_HIC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // BSC - Bright Star Catalogue, 5th Revised Ed. (Hoffleit+, 1991)
    FAIL(AddEntry(vobsCATALOG_BSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // SBSC - Supplement to the Bright Star Catalogue (Hoffleit+ 1983)
    FAIL(AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // B/sb9 - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // B/wds/wds - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // II/297/irc aka AKARI
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMidIR));

    return mcsSUCCESS;
}


/*___oOo___*/
