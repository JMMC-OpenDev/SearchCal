/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_K.cpp,v 1.2 2005-11-15 15:21:31 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K class.
 */

static char *rcsId="@(#) $Id: sclsvrSCENARIO_BRIGHT_K.cpp,v 1.2 2005-11-15 15:21:31 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
sclsvrSCENARIO_BRIGHT_K::sclsvrSCENARIO_BRIGHT_K()
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
    _request.Copy(*request);

    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();

    // Create a criteria list
    vobsSTAR_COMP_CRITERIA_LIST criteriaList;

    // Add criteria on right ascension
    if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST
    ///////////////////////////////////////////////////////////////////////////
   



// Temporary test



//            AddEntry(vobsCATALOG_MASS_ID,&_request, NULL, &_starListP, vobsCOPY);






    
    ///////////////////////////////////////////////////////////////////////////
    // I/280
    ///////////////////////////////////////////////////////////////////////////
    // Build the request for I/280
    _requestI280.Copy(_request);
    _requestI280.SetSearchBand("V");
    mcsFLOAT kMax = _request.GetMaxMagRange();
    mcsFLOAT vMax = kMax + 2.0;
    mcsFLOAT vMin = 0.0;
    _requestI280.SetMinMagRange(vMin);
    _requestI280.SetMaxMagRange(vMax);  
    // Build B-V filter
    _bvFilter.SetMagnitudeValue(1);
    _bvFilter.Enable();
    if (AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starListP,
                 vobsCOPY, NULL, &_bvFilter) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // 2MASS
    /////////////////////////////////////////////////////////////////////////// 
    _originFilter.SetOriginName(vobsCATALOG_MASS_ID ,vobsSTAR_PHOT_JHN_K);
    // Get research band
    mcsSTRING32 band;
    strcpy(band, _request.GetSearchBand());
    // Get 
    mcsFLOAT kMaxi = _request.GetMaxMagRange();
    mcsFLOAT kMini = _request.GetMinMagRange();
    _magnitudeFilter.SetMagnitudeValue(band, (kMaxi+kMini)/2, (kMaxi-kMini)/2);
    _originFilter.Enable();
    _magnitudeFilter.Enable();
    _filterList.Add(&_originFilter);
    _filterList.Add(&_magnitudeFilter);
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListP, &_starListP,
                 vobsCOPY, NULL, &_filterList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // BORDE
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    
    ///////////////////////////////////////////////////////////////////////////
    // MERAND
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, NULL, &_starListP, vobsMERGE) 
        == mcsFAILURE)
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
                 vobsCOPY) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    
    ///////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ///////////////////////////////////////////////////////////////////////////
    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
    // Change criteria on right ascension and declinaison
    if (criteriaList.Remove(vobsSTAR_POS_EQ_RA_MAIN) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (criteriaList.Remove(vobsSTAR_POS_EQ_RA_MAIN) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Add Criteria on coordinates
    if (criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        
    ///////////////////////////////////////////////////////////////////////////
    // LBSI
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    ///////////////////////////////////////////////////////////////////////////
    // MERAND
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // CHARM2
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // DENIS_JK
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add mgK criteria
    //if (criteriaList.Add(vobsSTAR_PHOT_JHN_K, 0.05) == mcsFAILURE)
      //{
      //return mcsFAILURE;
      //}

    ///////////////////////////////////////////////////////////////////////////
    // 2MASS
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Removed mgK criteria
    //if (criteriaList.Remove(vobsSTAR_PHOT_JHN_K) == mcsFAILURE)
      //{
      //return mcsFAILURE;
      //}

    // Add mgV criteria
    if (criteriaList.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // II/7A
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Removed mgV criteria
    if (criteriaList.Remove(vobsSTAR_PHOT_JHN_V) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    ///////////////////////////////////////////////////////////////////////////
    // II/225
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        

    // Add hd criteria
    if (criteriaList.Add(vobsSTAR_ID_HD, 0) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // I/196
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // BSC
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // SBSC
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    
    return mcsSUCCESS;
}

/*___oOo___*/
