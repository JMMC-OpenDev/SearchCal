/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of sclsvrSCENARIO_SINGLE_STAR class.
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
#include "sclsvrSCENARIO_SINGLE_STAR.h"
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_SINGLE_STAR::sclsvrSCENARIO_SINGLE_STAR(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_SINGLE_STAR::~sclsvrSCENARIO_SINGLE_STAR()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "SINGLE_STAR"
 */
const char* sclsvrSCENARIO_SINGLE_STAR::GetScenarioName() const
{
    return "SINGLE_STAR";
}

mcsCOMPL_STAT sclsvrSCENARIO_SINGLE_STAR::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // Skip scenario check execution while doing scenario dump:
    if (!vobsSCENARIO::vobsSCENARIO_DumpXML)
    {
        // Decisional scenario
        vobsSCENARIO scenarioCheck(_progress);
        // define catalog list:
        scenarioCheck.SetCatalogList(GetCatalogList());

        // Initialize it
        FAIL(scenarioCheck.AddEntry(vobsCATALOG_ASCC_ID, &_request, starList, starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

        // Run the method to execute the scenario which had been
        // loaded into memory
        FAIL_DO(scenarioCheck.Execute(ctx, _starList), errUserAdd(sclsvrERR_NO_CDS_RETURN));
    }
    else
    {
        ////////////////////////////////////////////////////////////////////////
        // I/280
        ////////////////////////////////////////////////////////////////////////

        // USELESS as it was already queried before:
        FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));
    }

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // I/311 - Hipparcos, the New Reduction (van Leeuwen, 2007)
    // to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // HIP1 - V / B / Ic (2013-04-18)
    FAIL(AddEntry(vobsCATALOG_HIP1_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // 2MASS
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2MASS));

    // I/355/gaiadr3
    FAIL(AddEntry(vobsCATALOG_GAIA_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaia));
    // I/355/paramp
    FAIL(AddEntry(vobsCATALOG_GAIA_AP_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecGaiaAP));

    // II/328/allwise aka WISE (LMN)
    FAIL(AddEntry(vobsCATALOG_WISE_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecWise));

    // DENIS_JK - J-K DENIS photometry of bright southern stars (Kimeswenger+ 2004)
    FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/7A - UBVRIJKLMNH Photoelectric Catalogue
    FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/225 - Catalog of Infrared Observations, Edition 5 (Gezari+ 1999)
    FAIL(AddEntry(vobsCATALOG_CIO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // I/196
    FAIL(AddEntry(vobsCATALOG_HIC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // BSC - Catalog of Infrared Observations, Edition 5 (Gezari+ 1999)
    FAIL(AddEntry(vobsCATALOG_BSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // SBSC - Supplement to the Bright Star Catalogue (Hoffleit+ 1983)
    FAIL(AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    if (vobsCATALOG_USNO_ID_ENABLE)
    {
        // I/284-USNO
        FAIL(AddEntry(vobsCATALOG_USNO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));
    }

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
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecAkari));

    // II/361 - MDFC
    FAIL(AddEntry(vobsCATALOG_MDFC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    return mcsSUCCESS;
}


/*___oOo___*/
