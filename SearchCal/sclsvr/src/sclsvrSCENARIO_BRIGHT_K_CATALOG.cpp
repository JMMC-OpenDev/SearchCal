/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_K_CATALOG.cpp,v 1.6 2011-03-03 13:12:52 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.5  2010/11/25 15:45:33  lafrasse
 * Removed CHARM querying from all scenarii.
 *
 * Revision 1.4  2010/01/26 14:16:06  lafrasse
 * Added WDS catalog querying.
 *
 * Revision 1.3  2010/01/22 15:35:29  lafrasse
 * Log refinments.
 *
 * Revision 1.2  2010/01/11 17:19:32  lafrasse
 * Changed magnitude filter band selection to an hardcoded 'K' instead of the '1'
 * from the request.
 *
 * Revision 1.1  2009/12/17 15:14:32  lafrasse
 * *** empty log message ***
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K_CATALOG class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_BRIGHT_K_CATALOG.cpp,v 1.6 2011-03-03 13:12:52 lafrasse Exp $"; 

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
/*   , _bvFilter("B-V filter")*/
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


/*
 * Protected methods
 */


/*
 * Private methods
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
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, alxARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, alxARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build criteria list on ra dec and V
    _criteriaListRaDecMagV.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_RA_MAIN, alxARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_DEC_MAIN, alxARCSEC_IN_DEGREES) ==
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
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_RA_MAIN, alxARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_DEC_MAIN, alxARCSEC_IN_DEGREES) ==
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
/*
    // Build B-V < 1 filter
    _bvFilter.SetMagnitudeValue(1);
    _bvFilter.Enable();
*/
    // Build origin = 2MASS for Kmag filter
    _originFilter.SetOriginName(vobsCATALOG_MASS_ID ,vobsSTAR_PHOT_JHN_K);
    _originFilter.Enable();
    // Build filter on magnitude
    // Get research band
    mcsDOUBLE kMaxi = _request.GetMaxMagRange();
    mcsDOUBLE kMini = _request.GetMinMagRange();
    _magnitudeFilter.SetMagnitudeValue("K", (kMaxi+kMini)/2, (kMaxi-kMini)/2);
    _magnitudeFilter.Enable();
    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");
    
    // PRIMARY REQUEST
    
    // I/280
/*
    if (AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starListP,
                 vobsCOPY, NULL, &_bvFilter, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
*/
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

    /* BORDE
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }*/
    
    /* MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }*/

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
