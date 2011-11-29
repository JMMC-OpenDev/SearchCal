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
sclsvrSCENARIO_FAINT_K::sclsvrSCENARIO_FAINT_K(sdbENTRY* progress): vobsSCENARIO(progress),
    _filterOptT("Opt = T filter", vobsSTAR_ID_CATALOG),
    _filterOptU("Opt = U filter", vobsSTAR_ID_CATALOG)
{
    // disable duplicates detection before the merge operation:
    _filterDuplicates = true;
    
    // disable star index use to perform faster merge operations:
    _enableStarIndex  = true;
    
    // disable flag to determine automatically the cone search radius for secondary requests using criteria radius
    _autoConeSearchRadius = true;
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
    if (InitCriteriaLists() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BUILD FILTER USED
    // Build Filter used opt=T
    if (_filterOptT.AddCondition(vobsEQUAL, "T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptT.Enable();

    // Build Filter used opt=T
    if (_filterOptU.AddCondition(vobsEQUAL, "U") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptU.Enable();

    // PRIMARY REQUEST

    // TODO: analyse primary requests to verify cross matching constraints (radius / criteria)

    // Get Radius entering by the user
    mcsDOUBLE radius;
    if (request->GetSearchArea(radius) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // if radius is not set (i.e equal zero)
    // compute radius from alx
    if (radius == 0.0)
    {
        mcsDOUBLE ra = request->GetObjectRaInDeg();
        mcsDOUBLE dec = request->GetObjectDecInDeg();

        mcsDOUBLE magMin = request->GetMinMagRange();
        mcsDOUBLE magMax = request->GetMaxMagRange();

        // compute radius with alx
        if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logTest("Sky research radius = %.2lf(arcmin)", radius);

        if (_request.SetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Decisionnal scenario
        vobsSCENARIO scenarioCheck(_progress);
        vobsSTAR_LIST starList;

        // Initialize it
        // Oct 2011: use _criteriaListRaDec to avoid duplicates:
        if (scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &starList,  vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Set catalog list
        vobsCATALOG_LIST catalogList;
        scenarioCheck.SetCatalogList(&catalogList);

        // Run the method to execute the scenario which had been
        // loaded into memory
        if (scenarioCheck.Execute(_starListP) == mcsFAILURE)
        {
            errUserAdd(sclsvrERR_NO_CDS_RETURN);
            return mcsFAILURE;
        }
        // If the return is lower than 25 star, twice the radius and recall
        // 2mass
        if (_starListP.Size() < 25)
        {
            if (_request.SetSearchArea(sqrt(2.0) * radius) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logTest("New Sky research radius = %.2lf(arcmin)", sqrt(2.0) * radius);

            // II/246
            // Oct 2011: use _criteriaListRaDec to avoid duplicates:
            if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // else if radius is defined, simply query 2mass
    else
    {
        logTest("Sky research radius = %.2lf(arcmin)", radius);

        // II/246
        // Oct 2011: use _criteriaListRaDec to avoid duplicates:
        if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP, vobsCOPY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Filter on opt=T
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, vobsCOPY, NULL, &_filterOptT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    // Filter on opt=U
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, vobsCOPY, NULL, &_filterOptU) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // query on I/280 with S1
    // I/280
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1, vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // query on I/284 with S2
    // I/284-UNSO
    if (AddEntry(vobsCATALOG_UNSO_ID, &_request, &_starListS2, &_starListS2, vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Merge S2 and S1
    // Oct 2011: use _criteriaListRaDec to avoid duplicates:
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1, vobsMERGE, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Update this new list S1 with P, S1 = reference list
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
 
    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // B/denis
    if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/wds/wds
    if (AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/297/irc aka AKARI
    if (AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS1, &_starListS1, vobsUPDATE_ONLY, &_criteriaListRaDecAkari) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*___oOo___*/
