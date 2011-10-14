/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K_CATALOG class.
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
 * SCALIB Headers 
 */
#include "alx.h"

/*
 * Local Headers 
 */
#include "sclsvrSCENARIO_BRIGHT_K_CATALOG.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_K_CATALOG::sclsvrSCENARIO_BRIGHT_K_CATALOG(sdbENTRY* progress):
    vobsSCENARIO(progress),
    _originFilter("K origin = 2mass filter"),
    _magnitudeFilter("K mag filter"),
    _filterList("filter List")
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_K_CATALOG::~sclsvrSCENARIO_BRIGHT_K_CATALOG()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "BRIGHT_K_CATALOG"
 */
const char* sclsvrSCENARIO_BRIGHT_K_CATALOG::GetScenarioName()
{
    return "BRIGHT_K_CATALOG";
}

/**
 * Initialize the BRIGHT K (JSDC) scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_K_CATALOG::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_K_CATALOG::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();
    
    // BUILD REQUEST USED
    // Build Genaral request
    _request.Copy(*request);
    
    // Build the request for I/280
    _requestI280.Copy(_request);
    _requestI280.SetSearchBand("V");
    
    mcsDOUBLE kMax = _request.GetMaxMagRange();
    mcsDOUBLE vMax = kMax + 2.0;
    mcsDOUBLE vMin = 0.0;
    _requestI280.SetMinMagRange(vMin);
    _requestI280.SetMaxMagRange(vMax);

    // BUILD CRITERIA LIST
    // Build criteria list on ra dec
    _criteriaListRaDec.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build criteria list on ra dec and V
    _criteriaListRaDecMagV.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add magV criteria
    if (_criteriaListRaDecMagV.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build criteria list on ra dec and hd
    _criteriaListRaDecHd.Clear();
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add hd criteria
    if (_criteriaListRaDecHd.Add(vobsSTAR_ID_HD) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //AKARI has a 2.4 HPBW for 9 and 18 mu, so 2 arc sec is necessary and OK
    _criteriaListRaDecAkari.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecAkari.Add(vobsSTAR_POS_EQ_RA_MAIN, 2. * sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecAkari.Add(vobsSTAR_POS_EQ_DEC_MAIN, 2. * sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // BUILD FILTER USED

    // Build origin = 2MASS for Kmag filter
    _originFilter.SetOriginName(vobsCATALOG_MASS_ID ,vobsSTAR_PHOT_JHN_K);
    _originFilter.Enable();
    
    // Build filter on magnitude
    // Get research band
    mcsDOUBLE kMaxi = _request.GetMaxMagRange();
    mcsDOUBLE kMini = _request.GetMinMagRange();
    _magnitudeFilter.SetMagnitudeValue("K", (kMaxi+kMini) / 2., (kMaxi-kMini) / 2.);
    _magnitudeFilter.Enable();
    
    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");
    
    // PRIMARY REQUEST
    
    // No criteriaRaDec for primary requests: duplicates are filtered at the first secondary request ...
    
    // I/280
    if (AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starListP, vobsCOPY, NULL, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListP, &_starListP, vobsCOPY, NULL, &_filterList, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, NULL, &_starListP, vobsMERGE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, NULL, &_starListP, vobsMERGE) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    // I/280 bis
    // TODO TEST duplicates within 1 arcsec: added _criteriaListRaDec
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListP, &_starListS, vobsCOPY, &_criteriaListRaDec, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // SECONDARY REQUEST
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
