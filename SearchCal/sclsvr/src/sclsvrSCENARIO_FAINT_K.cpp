/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_FAINT_K class.
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

#include "alx.h"
/*
 * Local Headers
 */
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrErrors.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_FAINT_K::sclsvrSCENARIO_FAINT_K(sdbENTRY* progress) : vobsSCENARIO(progress),
_starListP("Primary"), _starListS1("S1"), _starListS2("S2"),
_filterOptT("Opt = T filter", vobsSTAR_2MASS_OPT_ID_CATALOG),
_filterOptU("Opt = U filter", vobsSTAR_2MASS_OPT_ID_CATALOG)
{
    // disable duplicates detection because primary requests on 2MASS seems OK:
    SetRemoveDuplicates(false);
}

/**
 * Class destructor
 */
sclsvrSCENARIO_FAINT_K::~sclsvrSCENARIO_FAINT_K()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "FAINT_K"
 */
const char* sclsvrSCENARIO_FAINT_K::GetScenarioName() const
{
    return "FAINT_K";
}

/**
 * Initialize the FAINT K scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_FAINT_K::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS1.Clear();
    _starListS2.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // BUILD FILTER USED
    // Build Filter used opt=T
    FAIL(_filterOptT.AddCondition(vobsEQUAL, "T"));
    _filterOptT.Enable();

    // Build Filter used opt=T
    FAIL(_filterOptU.AddCondition(vobsEQUAL, "U"));
    _filterOptU.Enable();

    // PRIMARY REQUEST

    // Get Radius entering by the user
    mcsDOUBLE radius;
    FAIL(request->GetSearchArea(radius));

    // if radius is not set (i.e equal zero)
    // compute radius from alx
    if (radius == 0.0)
    {
        // compute radius with alx
        FAIL(alxGetResearchAreaSize(request->GetObjectRaInDeg(), request->GetObjectDecInDeg(),
                                    request->GetMinMagRange(), request->GetMaxMagRange(),
                                    &radius));

        logTest("Sky research radius=%.2lf arcmin", radius);

        FAIL(_request.SetSearchArea(radius));

        // Skip scenario check execution while doing scenario dump:
        if (!vobsSCENARIO::vobsSCENARIO_DumpXML)
        {
            // Decisional scenario
            vobsSCENARIO scenarioCheck(_progress);
            // define catalog list:
            scenarioCheck.SetCatalogList(GetCatalogList());

            // disable duplicates detection because primary requests on 2MASS seems OK:
            scenarioCheck.SetRemoveDuplicates(false);

            vobsSTAR_LIST starListCheck("Check");

            // Initialize it
            FAIL(scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &starListCheck, vobsCLEAR_MERGE, &_criteriaListRaDec2MASS));

            // Run the method to execute the scenario which had been
            // loaded into memory
            FAIL_DO(scenarioCheck.Execute(ctx, _starListP), errUserAdd(sclsvrERR_NO_CDS_RETURN));
        }

        // If the return is lower than 25 star, twice the radius and recall
        // 2mass
        if (_starListP.Size() < 25)
        {
            FAIL(_request.SetSearchArea(sqrt(2.0) * radius));

            logTest("New Sky research radius=%.2lf arcmin", sqrt(2.0) * radius);

            // II/246
            FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCLEAR_MERGE, &_criteriaListRaDec2MASS));
        }
    }
    else
    {
        // else if radius is defined, simply query 2mass
        logTest("Sky research radius=%.2lf arcmin", radius);

        // II/246
        FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCLEAR_MERGE, &_criteriaListRaDec2MASS));
    }

    // Note: Oct 2012: rewriting Faint scenario

    // list P contains 2MASS results

    // S1 = Tycho stars:
    // Filter on opt=T in list S1 (T)
    // use vobsCLEAR_MERGE to move 2MASS (Tycho stars) into S1 (T):
    FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, vobsCLEAR_MERGE, &_criteriaListRaDec, &_filterOptT));

    // query on I/280 with list S1 = 2MASS (Tycho stars) (T)
    // use vobsUPDATE_ONLY to merge 2MASS (epoch) with ASCC (pmRa/Dec)
    // I/280
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // I/311 to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // HIP1 - V / B / Ic (2013-04-18)
    FAIL(AddEntry(vobsCATALOG_HIP1_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecHip));

    // I/345/gaia2
    FAIL(AddEntry(vobsCATALOG_GAIA_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecGaia));
    // I/347/gaia2dis
    FAIL(AddEntry(vobsCATALOG_GAIA_DIST_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecGaiaDist));

    // 2017.4: disable USNO as new approach can not compute diameters with photographic mags:
    if (vobsCATALOG_USNO_ID_ENABLE)
    {
        // S2 = Usno stars:
        // Filter on opt=U in list S2 = 2MASS (Usno stars) (U)
        // use vobsCLEAR_MERGE to move 2MASS (Usno stars) into S2 (U):
        FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, vobsCLEAR_MERGE, &_criteriaListRaDec, &_filterOptU));

        // query on I/284 with list S2
        // use vobsUPDATE_ONLY to merge 2MASS (epoch) with USNO (pmRa/Dec)
        // I/284-USNO
        FAIL(AddEntry(vobsCATALOG_USNO_ID, &_request, &_starListS2, &_starListS2, vobsUPDATE_ONLY, &_criteriaListRaDec));

        // Merge S1 and S2 into list S1
        // ie obtain all initial 2MASS (Tycho stars with ASCC + HIP2 and Usno stars)
        FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1, vobsMERGE, &_criteriaListRaDec));
    }

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // II/328/allwise aka WISE (LMN)
    FAIL(AddEntry(vobsCATALOG_WISE_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecMidIR));

    if (vobsCATALOG_DENIS_ID_ENABLE)
    {
        // B/denis - so far not able to use Denis properly
        FAIL(AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));
    }

    // B/sb9 - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // B/wds/wds - Washington Visual Double Star Catalog (Mason+ 2001-2013)
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec2));

    // II/297/irc aka AKARI
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecMidIR));

    return mcsSUCCESS;
}


/*___oOo___*/
