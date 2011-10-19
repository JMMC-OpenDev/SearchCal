/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_LBSI class definition.
 * 
 * The LBSI catalog ["J/A+A/393/183/catalog"] is used in primary and secondary requests for BRIGHT K and V scenarios 
 * to get magnitudes and UDDK diameter
 * 
 * J/A+A/393/183/catalog
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdio.h>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_LBSI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_LBSI::vobsCATALOG_LBSI() : vobsREMOTE_CATALOG(vobsCATALOG_LBSI_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_LBSI::~vobsCATALOG_LBSI()
{
}


/*
 * Private methods
 */

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return always mcsSUCCESS
 */
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT K and V scenarios

    // Get the johnson magnitude Bmag (PHOT_JHN_B) stored in the 'vobsSTAR_PHOT_JHN_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");
    
    // Get the johnson magnitude Vmag (PHOT_JHN_V) stored in the 'vobsSTAR_PHOT_JHN_V' property
    miscDynBufAppendString(&_query, "&-out=Vmag");
    
    // Get the johnson magnitude Jmag (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=Jmag");
    
    // Get the johnson magnitude Hmag (PHOT_JHN_H) stored in the 'vobsSTAR_PHOT_JHN_H' property
    miscDynBufAppendString(&_query, "&-out=Hmag");
    
    // Get the johnson magnitude Kmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Kmag");
    
    // Get the johnson magnitude Lmag (PHOT_JHN_L) stored in the 'vobsSTAR_PHOT_JHN_L' property
    miscDynBufAppendString(&_query, "&-out=Lmag");
    
    // Get the johnson magnitude Mmag (PHOT_JHN_M) stored in the 'vobsSTAR_PHOT_JHN_M' property
    miscDynBufAppendString(&_query, "&-out=Mmag");
    
    // Get the uniform disk diameter UDDK (EXTENSION_DIAM) stored in the 'vobsSTAR_UDDK_DIAM' property
    miscDynBufAppendString(&_query, "&-out=UDDK");
    
    // Get the uniform disk diameter error e_UDDK (ERROR) stored in the 'vobsSTAR_UDDK_DIAM_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_UDDK");
    
    return mcsSUCCESS;
}

/**
 * Build the specific part of the asking.
 *
 * Build the specific part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(vobsREQUEST &request)
{
    // TODO: factorize duplicated code
    
    // Add band constraint
    const char* band = request.GetSearchBand();
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange = request.GetMinMagRange();
    mcsDOUBLE maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2lf..%.2lf", minMagRange, maxMagRange);

    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0lf/%.0lf", deltaRa, deltaDec);

    // Add query constraints:
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
