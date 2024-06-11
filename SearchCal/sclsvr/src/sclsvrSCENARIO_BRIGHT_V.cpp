/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_V class.
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
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_V::sclsvrSCENARIO_BRIGHT_V(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_V::~sclsvrSCENARIO_BRIGHT_V()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "BRIGHT_V"
 */
const char* sclsvrSCENARIO_BRIGHT_V::GetScenarioName() const
{
    return "BRIGHT_V";
}

/**
 * Initialize the BRIGHT V scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_V::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // PRIMARY REQUEST

    // I/280
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDec));

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

    // DENIS_JK - J-K DENIS photometry of bright southern stars (Kimeswenger+ 2004)
    FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/7A - UBVRIJKLMNH Photoelectric Catalogue
    FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/225 - Catalog of Infrared Observations, Edition 5 (Gezari+ 1999)
    FAIL(AddEntry(vobsCATALOG_CIO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // I/196
    FAIL(AddEntry(vobsCATALOG_HIC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // BSC - Bright Star Catalogue, 5th Revised Ed. (Hoffleit+, 1991)
    FAIL(AddEntry(vobsCATALOG_BSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd))

    // SBSC - Supplement to the Bright Star Catalogue (Hoffleit+ 1983)
    FAIL(AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    if (vobsCATALOG_DENIS_ID_ENABLE)
    {
        // B/denis - so far not able to use Denis properly
        FAIL(AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));
    }

    // B/sb9 - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // B/wds/wds - Washington Visual Double Star Catalog (Mason+ 2001-2013)
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // II/297/irc aka AKARI
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMidIR));

    return mcsSUCCESS;
}


/*___oOo___*/
