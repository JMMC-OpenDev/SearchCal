/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K class.
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
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_K::sclsvrSCENARIO_BRIGHT_K(sdbENTRY* progress):
    vobsSCENARIO(progress),
    _originFilter("K origin = 2mass filter"),
    _magnitudeFilter("K mag filter"),
    _filterList("filter List")
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_K::~sclsvrSCENARIO_BRIGHT_K()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "BRIGHT_K"
 */
const char* sclsvrSCENARIO_BRIGHT_K::GetScenarioName()
{
    return "BRIGHT_K";
}

/**
 * Initialize the BRIGHT K scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_K::Init(vobsREQUEST* request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_K::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();
    
    // BUILD REQUEST USED
    // Build General request
    _request.Copy(*request);
    
    // Build the request for I/280
    _requestI280.Copy(_request);
//      _requestI280.SetSearchBand("K");
     _requestI280.SetSearchBand("V");
     
    mcsDOUBLE kMax = _request.GetMaxMagRange();

    mcsDOUBLE vMax = kMax + 2.0;
    mcsDOUBLE vMin = 0.0;
    _requestI280.SetMinMagRange(vMin);
    _requestI280.SetMaxMagRange(vMax);

    // BUILD CRITERIA LIST
    if (InitCriteriaLists() == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // BUILD FILTER USED
    // Build origin = 2MASS for Kmag filter
     _originFilter.SetOriginName(vobsCATALOG_MASS_ID ,vobsSTAR_PHOT_JHN_K);
     _originFilter.Enable();
     
    // Build filter on magnitude
    // Get research band
    mcsSTRING32 band;
    strcpy(band, _request.GetSearchBand());
    mcsDOUBLE kMaxi = _request.GetMaxMagRange();
    mcsDOUBLE kMini = _request.GetMinMagRange();
    _magnitudeFilter.SetMagnitudeValue(band, (kMaxi+kMini) / 2., (kMaxi-kMini) / 2.);
    _magnitudeFilter.Enable();
    
    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");

    // PRIMARY REQUEST
    
    // I/280
    if (AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starListP, vobsCOPY, &_criteriaListRaDec, NULL, "&SpType=%5bOBAFGKM%5d*&e_Plx=%3E0.0&Plx=%3E0.999") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListP, &_starListP, vobsCOPY, &_criteriaListRaDec, &_filterList, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BORDE
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, NULL, &_starListP, vobsMERGE, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    // MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, NULL, &_starListP, vobsMERGE, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, NULL, &_starListP, vobsMERGE, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, NULL, &_starListP, vobsMERGE, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    // I/280 bis
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListP, &_starListS, vobsCOPY, &_criteriaListRaDec, NULL, "&SpType=%5bOBAFGKM%5d*&e_Plx=%3E0.0&Plx=%3E0.999") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // SECONDARY REQUEST
    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, II/7A, BSC, SBSC, DENIS
    // LBSI
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // DENIS_JK
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec, NULL, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDecMagV) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        
    // I/196
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BSC
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // SBSC
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/wsd/wsd
    if (AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/297/irc aka AKARI
    if (AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS, &_starListS, vobsUPDATE_ONLY, &_criteriaListRaDecAkari) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}


/*___oOo___*/
