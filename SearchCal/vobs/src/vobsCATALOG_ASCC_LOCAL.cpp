/*******************************************************************************
 * JMMC project
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_ASCC_LOCAL class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_ASCC_LOCAL.cpp,v 1.1.2.1 2011-04-15 22:19:32 duvert Exp $"; 

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
#include "math.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_ASCC_LOCAL.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 */
vobsCATALOG_ASCC_LOCAL::vobsCATALOG_ASCC_LOCAL() : vobsLOCAL_CATALOG("ASCC_LOCAL", 
//                                                          "vobsAsccExcerptForJSDC.cfg")
//                                                          "test.cfg")
                                                         "testmagakari.cfg")
{
}

/**
 * Class destructor
 */
vobsCATALOG_ASCC_LOCAL::~vobsCATALOG_ASCC_LOCAL()
{
}

/*
 * Public methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT vobsCATALOG_ASCC_LOCAL::Search(vobsREQUEST &request,
					     vobsSTAR_LIST &list)
{
    logTrace("vobsCATALOG_ASCC_LOCAL::Search()");
    //
    // Load catalog in star list
    // -------------------------
    if (Load() == mcsFAILURE)
    {
        // Add error with specifying the catalog name
        errAdd(vobsERR_CATALOG_LOAD, GetName());
        return mcsFAILURE;
    }
    logTest("Catalog _ASCC_LOCAL correctly loaded in a star list");

    //just copy everything
    for (mcsUINT32 i=0; i<_starList.Size(); i++)
    {
        // Get catalog star
        vobsSTAR *asccCatalogStarPtr;
        asccCatalogStarPtr = _starList.GetNextStar((mcsLOGICAL)(i==0));
	list.AddAtTail(*asccCatalogStarPtr);
    }
    
    return mcsSUCCESS;    
}    
/**
 * Load ASCC_LOCAL catalog.
 *
 * Build star list from ASCC_LOCAL catalog stars.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_ASCC_LOCAL::Load(void)
{
    logTrace("vobsCATALOG_ASCC_LOCAL::Load()");
    
    if (_loaded == mcsFALSE)
    {
        //
        // Standard procedure to load catalog
        // ----------------------------------
        if (vobsLOCAL_CATALOG::Load() == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        //
        // ASCC_LOCAL specific loading actions
        // -----------------------------

//         // Compute magnitude in N band
//         mcsUINT32 starIdx;
//         for (starIdx = 0; starIdx < _starList.Size(); starIdx++)
//         {
//             // Get star
//             vobsSTAR *starPtr;
//             starPtr = _starList.GetNextStar((mcsLOGICAL)(starIdx==0));

//             // Get IR flux
//             mcsDOUBLE flux;
//             starPtr->GetPropertyValue(vobsSTAR_PHOT_JHN_K, &flux);
//         }

    }
  
    return mcsSUCCESS;
}

/*___oOo___*/
