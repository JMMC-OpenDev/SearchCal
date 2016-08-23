/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_JSDC_FAINT_LOCAL class.
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
#include "vobsCATALOG_JSDC_FAINT_LOCAL.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/**
 * Class constructor
 */
vobsCATALOG_JSDC_FAINT_LOCAL::vobsCATALOG_JSDC_FAINT_LOCAL() : vobsLOCAL_CATALOG(vobsCATALOG_JSDC_FAINT_LOCAL_ID,
                                                                                 /* "vobsascc_simbad_south.cfg") */
                                                                                 "vobsascc_simbad_sptype.cfg")
{
}

/**
 * Class destructor
 */
vobsCATALOG_JSDC_FAINT_LOCAL::~vobsCATALOG_JSDC_FAINT_LOCAL()
{
}

/*
 * Public methods
 */

/*
 * Private methods
 */
mcsCOMPL_STAT vobsCATALOG_JSDC_FAINT_LOCAL::Search(vobsSCENARIO_RUNTIME &ctx,
                                                   vobsREQUEST &request,
                                                   vobsSTAR_LIST &list,
                                                   const char* option,
                                                   vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap,
                                                   mcsLOGICAL logResult)
{
    //
    // Load catalog in star list
    // -------------------------
    FAIL_DO(Load(propertyCatalogMap), errAdd(vobsERR_CATALOG_LOAD, GetName()));

    // Sort by declination to optimize CDS queries because spatial index(dec) is probably in use
    _starList.Sort(vobsSTAR_POS_EQ_DEC_MAIN);

    // just move stars into given list:
    list.CopyRefs(_starList);

    // Free memory (internal loaded star list corresponding to the complete local catalog)
    Clear();

    logTest("CATALOG_JSDC_FAINT_LOCAL correctly loaded: %d stars", list.Size());

    return mcsSUCCESS;
}

/**
 * Load JSDC_FAINT_LOCAL catalog.
 *
 * Build star list from JSDC_FAINT_LOCAL catalog stars.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT vobsCATALOG_JSDC_FAINT_LOCAL::Load(vobsCATALOG_STAR_PROPERTY_CATALOG_MAPPING* propertyCatalogMap)
{
    if (IS_FALSE(_loaded))
    {
        //
        // Standard procedure to load catalog
        // ----------------------------------
        FAIL(vobsLOCAL_CATALOG::Load(propertyCatalogMap));
    }

    return mcsSUCCESS;
}

/*___oOo___*/
