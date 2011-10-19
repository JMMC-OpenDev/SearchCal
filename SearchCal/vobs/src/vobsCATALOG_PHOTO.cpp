/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_PHOTO class definition.
 * 
 * The PHOTO catalog ["II/7A/catalog"] is used in primary and secondary requests for BRIGHT scenarios 
 * to get johnson magnitudes
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
#include "vobsCATALOG_PHOTO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_PHOTO::vobsCATALOG_PHOTO() : vobsREMOTE_CATALOG(vobsCATALOG_PHOTO_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_PHOTO::~vobsCATALOG_PHOTO()
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios
    
    // Get the johnson magnitude B (PHOT_JHN_B) stored in the 'vobsSTAR_PHOT_JHN_B' property
    miscDynBufAppendString(&_query, "&-out=B");

    // Get the johnson magnitude V (PHOT_JHN_V) stored in the 'vobsSTAR_PHOT_JHN_V' property
    miscDynBufAppendString(&_query, "&-out=V");

    // Get the johnson magnitude R (PHOT_JHN_R) stored in the 'vobsSTAR_PHOT_JHN_R' property
    miscDynBufAppendString(&_query, "&-out=R");

    // Get the johnson magnitude I (PHOT_JHN_I) stored in the 'vobsSTAR_PHOT_JHN_I' property
    miscDynBufAppendString(&_query, "&-out=I");

    // Get the johnson magnitude J (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=J");

    // missing H band
    
    // Get the johnson magnitude K (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=K");

    // Get the johnson magnitude L (PHOT_JHN_L) stored in the 'vobsSTAR_PHOT_JHN_L' property
    miscDynBufAppendString(&_query, "&-out=L");

    // Get the johnson magnitude M (PHOT_JHN_M) stored in the 'vobsSTAR_PHOT_JHN_M' property
    miscDynBufAppendString(&_query, "&-out=M");

    // TODO: why not get the magitudes in H and N bands ?
            
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
 * @return always mcsSUCCESS 
 */
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(vobsREQUEST &request)
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
    // parameter '&X=' 
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
