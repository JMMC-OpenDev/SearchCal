/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_K.cpp,v 1.20 2011-03-03 13:12:52 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.19  2010/11/24 15:27:27  lafrasse
 * Removed CHARM querying from Bright K scenario.
 *
 * Revision 1.18  2010/10/15 13:33:19  lafrasse
 * Removed "B-V<1" filter.
 *
 * Revision 1.17  2010/01/26 14:16:06  lafrasse
 * Added WDS catalog querying.
 *
 * Revision 1.16  2009/02/26 10:58:31  lafrasse
 * Changed cross-matching method for SB9 catalog.
 *
 * Revision 1.15  2009/02/09 16:28:25  lafrasse
 * Added spectral binary detection (SB9 catalog).
 *
 * Revision 1.14  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.13  2006/03/28 12:58:13  scetre
 * Added parameters constraints on I/280 query
 *
 * Revision 1.12  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.11  2006/01/18 08:49:40  scetre
 * Removed unused filter on Qflg and opt because they are now managed by the catalog option
 *
 * Revision 1.10  2005/12/14 09:02:35  scetre
 * Changed constructor in order to handle the filter of the scenario classes
 *
 * Revision 1.9  2005/12/12 14:11:26  scetre
 * Fiwed bug with filter entry
 *
 * Revision 1.8  2005/12/07 14:51:42  scetre
 * Used new generic filter
 *
 * Revision 1.7  2005/11/30 10:35:21  scetre
 * Updated Filter without name
 * Updated scenario
 *
 * Revision 1.6  2005/11/29 10:39:03  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.5  2005/11/24 13:19:37  scetre
 * Clean the presentation of the add entry
 *
 * Revision 1.4  2005/11/18 10:08:50  scetre
 * Removed old scenario
 *
 * Revision 1.3  2005/11/16 14:28:02  scetre
 * Used criteria list as member of the class. All necessary criteria list are now built at the beginning of the Init() method
 *
 * Revision 1.2  2005/11/15 15:21:31  scetre
 * Removed unused printf
 *
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_BRIGHT_K.cpp,v 1.20 2011-03-03 13:12:52 lafrasse Exp $"; 

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


/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_K::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_K::Init()");

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
    _magnitudeFilter.SetMagnitudeValue(band, (kMaxi+kMini)/2, (kMaxi-kMini)/2);
    _magnitudeFilter.Enable();
    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");
    
    // PRIMARY REQUEST
    
    // I/280
    if (AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starListP,
                 vobsCOPY, NULL, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListP, &_starListP,
                 vobsCOPY, NULL, &_filterList, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BORDE
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    // MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, NULL, &_starListP, vobsMERGE) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    // I/280 bis
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListP, &_starListS,
                 vobsCOPY, NULL, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // SECONDARY REQUEST
    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, II/7A, BSC, SBSC, DENIS
    // LBSI
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // DENIS_JK
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec, NULL,
                          "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecMagV) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        
    // I/196
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BSC
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // SBSC
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/wsd/wsd
    if (AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
