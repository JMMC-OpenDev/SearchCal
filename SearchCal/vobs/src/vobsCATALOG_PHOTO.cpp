/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_PHOTO class definition.
 */


/* 
 * System Headers 
 */
#include <iostream>
#include <stdio.h>
using namespace std; 
/**
 * Export standard iostream objects (cin, cout,...).
 */

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

/**
 * Build a catalog object.
 */
vobsCATALOG_PHOTO::vobsCATALOG_PHOTO() : vobsREMOTE_CATALOG(vobsCATALOG_PHOTO_ID)
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_PHOTO::~vobsCATALOG_PHOTO()
{
}

/*
 * Protected methods
 */

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return always mcsSUCCESS
 *
 */
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(void)
{
    // properties to retrieve
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_M");

    // order by distance
    miscDynBufAppendString(&_query, "&-sort=_r");
            
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return always mcsSUCCESS 
 *
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
    // TODO: why arcmin and not arcsec ??
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}

/*___oOo___*/
