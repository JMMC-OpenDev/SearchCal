/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_FAINT_K class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.26.2.1 2011-04-08 19:34:53 duvert Exp $"; 

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
sclsvrSCENARIO_FAINT_K::sclsvrSCENARIO_FAINT_K(sdbENTRY* progress):
    vobsSCENARIO(progress),
    _filterOptT("Opt = T filter", vobsSTAR_ID_CATALOG),
    _filterOptU("Opt = U filter", vobsSTAR_ID_CATALOG)
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
 * Initialize the faint K scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_FAINT_K::Init(vobsREQUEST * request)
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
    _criteriaListRaDec.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) == mcsFAILURE)
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

    // Get Radius entering by the user
    mcsDOUBLE radius;
    if (request->GetSearchArea(radius) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // if radius is not set (i.e equal zero)
    // compute radius from alx
    if (radius == 0)
    {
        mcsSTRING32 raString;
        strcpy(raString, request->GetObjectRa());
        mcsSTRING32 decString;
        strcpy(decString, request->GetObjectDec());
        vobsSTAR star;
        star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, raString, "no origin");
        star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, decString, "no origin");

        mcsDOUBLE ra;
        mcsDOUBLE dec;
        star.GetRa(ra);
        star.GetDec(dec);
        
        mcsDOUBLE magMin;
        mcsDOUBLE magMax;
        magMin = request->GetMinMagRange();
        magMax = request->GetMaxMagRange();
        
        // compute radius with alx
        if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &radius) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logInfo("Sky research radius = %.2f(arcmin)", radius);
        
        if (_request.SetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Decisionnal scenario
        vobsSCENARIO scenarioCheck(_progress);
        vobsSTAR_LIST starList;
        // Initialize it
        if (scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL,
                                   &starList, vobsCOPY, NULL, NULL,
                                   "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
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
            if (_request.SetSearchArea(sqrt(2.0)*radius) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logInfo("New Sky research radius = %.2f(arcmin)", sqrt(2.0)*radius);

            // II/246
            if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                         vobsCOPY, NULL, NULL, 
                         "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // else if radius is defined, simply query 2mass
    else
    {
        // II/246
        if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                     vobsCOPY, NULL, NULL, 
                     "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Filter on opt=T
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, 
                vobsCOPY, NULL, &_filterOptT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    // Filter on opt=U
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, 
                vobsCOPY, NULL, &_filterOptU) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // query on I/280 with S1
    // I/280 with 
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // query on I/284 with S2
    // I/284-UNSO
    if (AddEntry(vobsCATALOG_UNSO_ID, &_request, &_starListS2, &_starListS2,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Merge S2 and S1
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1,
                 vobsMERGE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Update this new list S1 with P, S1 = reference list
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/denis
    if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS1, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/wsd/wsd
    if (AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS1, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // II/297/irc aka AKARI
    if (AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS1, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
