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
_filterOptT("Opt = T filter", vobsSTAR_2MASS_OPT_ID_CATALOG),
_filterOptU("Opt = U filter", vobsSTAR_2MASS_OPT_ID_CATALOG)
{
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
const char* sclsvrSCENARIO_FAINT_K::GetScenarioName()
{
    return "FAINT_K";
}

/**
 * Initialize the FAINT K scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_FAINT_K::Init(vobsREQUEST* request)
{
    logTrace("sclsvrSCENARIO_FAINT_K::Init()");

    // Clear the scenario
    Clear();
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

    // TODO: analyse primary requests to verify cross matching constraints (radius / criteria)

    // Get Radius entering by the user
    mcsDOUBLE radius;
    FAIL(request->GetSearchArea(radius));

    // if radius is not set (i.e equal zero)
    // compute radius from alx
    if (radius == 0.0)
    {
        mcsDOUBLE ra = request->GetObjectRaInDeg();
        mcsDOUBLE dec = request->GetObjectDecInDeg();

        mcsDOUBLE magMin = request->GetMinMagRange();
        mcsDOUBLE magMax = request->GetMaxMagRange();

        // compute radius with alx
        FAIL(alxGetResearchAreaSize(ra, dec, magMin, magMax, &radius));

        logTest("Sky research radius = %.2lf(arcmin)", radius);

        FAIL(_request.SetSearchArea(radius));

        // Decisionnal scenario
        vobsSCENARIO scenarioCheck(_progress);
        vobsSTAR_LIST starList;

        // Initialize it
        // Oct 2011: use _criteriaListRaDec to avoid duplicates:
        FAIL(scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &starList, vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA"));

        // Set catalog list
        vobsCATALOG_LIST catalogList;
        scenarioCheck.SetCatalogList(&catalogList);

        // Run the method to execute the scenario which had been
        // loaded into memory
        FAIL_DO(scenarioCheck.Execute(_starListP), errUserAdd(sclsvrERR_NO_CDS_RETURN));

        // If the return is lower than 25 star, twice the radius and recall
        // 2mass
        if (_starListP.Size() < 25)
        {
            FAIL(_request.SetSearchArea(sqrt(2.0) * radius));

            logTest("New Sky research radius = %.2lf(arcmin)", sqrt(2.0) * radius);

            // II/246
            // Oct 2011: use _criteriaListRaDec to avoid duplicates:
            FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA"));
        }
    }
    else
    {
        // else if radius is defined, simply query 2mass
        logTest("Sky research radius = %.2lf(arcmin)", radius);

        // II/246
        // Oct 2011: use _criteriaListRaDec to avoid duplicates:
        FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA"));
    }

    // Filter on opt=T
    FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, vobsCOPY, NULL, &_filterOptT));

    // Filter on opt=U
    FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, vobsCOPY, NULL, &_filterOptU));

    // query on I/280 with S1
    // I/280
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1, vobsCOPY, &_criteriaListRaDec));

    // query on I/284 with S2
    // I/284-USNO
    FAIL(AddEntry(vobsCATALOG_USNO_ID, &_request, &_starListS2, &_starListS2, vobsCOPY, &_criteriaListRaDec));

    // Merge S2 and S1
    // Oct 2011: use _criteriaListRaDec to avoid duplicates:
    FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1, vobsMERGE, &_criteriaListRaDec));

    // Update this new list S1 with P, S1 = reference list
    FAIL(AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // B/denis
    FAIL(AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // B/sb9
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // B/wds/wds
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/297/irc aka AKARI
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecAkari));

    return mcsSUCCESS;
}


/*___oOo___*/
