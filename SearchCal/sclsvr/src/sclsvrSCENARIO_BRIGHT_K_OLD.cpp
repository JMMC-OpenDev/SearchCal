/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_K_OLD.cpp,v 1.5 2006-07-17 09:10:36 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.3  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.2  2005/12/14 09:02:35  scetre
 * Changed constructor in order to handle the filter of the scenario classes
 *
 * Revision 1.1  2005/12/12 14:07:57  scetre
 * Added old scenario with 2mass in primary request
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of sclsvrSCENARIO_BRIGHT_K_OLD class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_BRIGHT_K_OLD.cpp,v 1.5 2006-07-17 09:10:36 scetre Exp $"; 

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
#include "sclsvrSCENARIO_BRIGHT_K_OLD.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_K_OLD::sclsvrSCENARIO_BRIGHT_K_OLD():
_originFilter("K origin = 2mass filter"),
    _magnitudeFilter("K mag filter"),
    _filterList("filter List"),
    _bvFilter("B-V filter"),
    _filterOptT("Opt = T filter", vobsSTAR_ID_CATALOG)
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_K_OLD::~sclsvrSCENARIO_BRIGHT_K_OLD()
{
}

/*
 * Public methods
 */


/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_K_OLD::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_K_OLD::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();
    
    //////////////////////////////////////////////////////////////////////////
    // BUILD REQUEST USED
    //////////////////////////////////////////////////////////////////////////  
    // Build Genaral request
    _request.Copy(*request);
    // Build the request for I/280
    _requestI280.Copy(_request);
    _requestI280.SetSearchBand("V");
    mcsFLOAT kMax = _request.GetMaxMagRange();
    mcsFLOAT vMax = kMax + 2.0;
    mcsFLOAT vMin = 0.0;
    _requestI280.SetMinMagRange(vMin);
    _requestI280.SetMaxMagRange(vMax);

    //////////////////////////////////////////////////////////////////////////
    // BUILD CRITERIA LIST
    //////////////////////////////////////////////////////////////////////////
    // Build criteria list on ra dec
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
    // Build criteria list on ra dec and V
    _criteriaListRaDecMagV.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add mgV criteria
    if (_criteriaListRaDecMagV.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Build criteria list on ra dec and hd
    _criteriaListRaDecHd.Clear();
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add hd criteria
    if (_criteriaListRaDecHd.Add(vobsSTAR_ID_HD, 0) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    //////////////////////////////////////////////////////////////////////////
    // BUILD FILTER USED
    //////////////////////////////////////////////////////////////////////////
    // Filter on opt=T
    if (_filterOptT.AddCondition(vobsEQUAL, "T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptT.Enable();
    // Build B-V < 1 filter
    _bvFilter.SetMagnitudeValue(1);
    _bvFilter.Enable();
    // Build origin = 2MASS for Kmag filter
    _originFilter.SetOriginName(vobsCATALOG_MASS_ID ,vobsSTAR_PHOT_JHN_K);
    _originFilter.Enable();
    // Build filter on magnitude
    // Get research band
    mcsSTRING32 band;
    strcpy(band, _request.GetSearchBand());
    mcsFLOAT kMaxi = _request.GetMaxMagRange();
    mcsFLOAT kMini = _request.GetMinMagRange();
    _magnitudeFilter.SetMagnitudeValue(band, (kMaxi+kMini)/2, (kMaxi-kMini)/2);
    _magnitudeFilter.Enable();
    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");
    
    ///////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    // 2MASS
    /////////////////////////////////////////////////////////////////////////// 
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                 vobsCOPY, NULL, NULL, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // II/225
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, NULL, &_starListP, vobsMERGE) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // II/7A
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    ///////////////////////////////////////////////////////////////////////////
    // I/280 bis
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListP, &_starListS,
                 vobsCOPY, NULL, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ///////////////////////////////////////////////////////////////////////////
    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
    ///////////////////////////////////////////////////////////////////////////
    // LBSI
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // MERAND
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // CHARM2
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // DENIS_JK
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // 2MASS
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec, NULL,
                          "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // II/7A
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecMagV) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // II/225
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        
    ///////////////////////////////////////////////////////////////////////////
    // I/196
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // BSC
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // SBSC
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    
    return mcsSUCCESS;
}

/*___oOo___*/
