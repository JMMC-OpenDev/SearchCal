/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_ASCC_LOCAL class.
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdlib.h>
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
vobsCATALOG_ASCC_LOCAL::vobsCATALOG_ASCC_LOCAL() : vobsLOCAL_CATALOG(vobsCATALOG_ASCC_ID,
                                                                     "vobsascc.cfg")
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
                                             vobsSTAR_LIST &list,
                                             vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                             mcsLOGICAL logResult)
{
    //
    // Load catalog in star list
    // -------------------------
    FAIL_DO(Load(propertyCatalogMap), errAdd(vobsERR_CATALOG_LOAD, GetName()));

    // Sort by declination to optmize CDS queries because spatial index(dec) is probably in use
    _starList.Sort(vobsSTAR_POS_EQ_DEC_MAIN);

    // just move stars into given list:
    list.CopyRefs(_starList);

    // Free memory (internal loaded star list corresponding to the complete local catalog)
    Clear();

    logTest("CATALOG_ASCC_LOCAL correctly loaded: %d stars", list.Size());

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
mcsCOMPL_STAT vobsCATALOG_ASCC_LOCAL::Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap)
{
    if (_loaded == mcsFALSE)
    {
        //
        // Standard procedure to load catalog
        // ----------------------------------
        FAIL(vobsLOCAL_CATALOG::Load(propertyCatalogMap));
    }

    return mcsSUCCESS;
}

/*___oOo___*/
