/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_MERAND class.
 * 
 * The MERAND catalog ["J/A+A/433/1155"] is used in primary and secondary requests for BRIGHT K and V scenarios 
 * to get magnitudes and UDDK diameter
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdio.h>
#include <string.h>
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
#include "vobsCATALOG_MERAND.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_MERAND::vobsCATALOG_MERAND() : vobsREMOTE_CATALOG(vobsCATALOG_MERAND_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_MERAND::~vobsCATALOG_MERAND()
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
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(void)
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
    
    // Get the johnson magnitude Ksmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    
    // Get the uniform disk diameter UDdiamKs (EXTENSION_DIAM) stored in the 'vobsSTAR_UDDK_DIAM' property
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");
    
    // Get the uniform disk diameter error e_UDdiam (ERROR) stored in the 'vobsSTAR_UDDK_DIAM_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");
    
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
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(vobsREQUEST &request)
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
    miscDynBufAppendString(&_query, (strcmp(band, "K") == 0) ? "smag=" : "mag="); // Ksmag= or Xmag=
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
