/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_MASS class definition.
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
#include "vobsCATALOG_MASS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_MASS::vobsCATALOG_MASS() : vobsREMOTE_CATALOG(vobsCATALOG_MASS_ID)
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_MASS::~vobsCATALOG_MASS()
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
 *
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(void)
{
    // properties to retrieve
    miscDynBufAppendString(&_query, "&-out=2MASS");    
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=*ID_CATALOG");
    miscDynBufAppendString(&_query, "&-out=*CODE_QUALITY");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LAT");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LON");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_V");
    
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(vobsREQUEST &request)
{
    // TODO: factorize duplicated code
    
    // Add band constraint
    const char* band = request.GetSearchBand();
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange = request.GetMinMagRange();
    mcsDOUBLE maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2lf..%.2lf", minMagRange, maxMagRange);

    mcsSTRING32 separation;
    const char* geom;
    // Add search box size
    if (request.GetSearchAreaGeometry() == vobsBOX)
    {
        mcsDOUBLE deltaRa;
        mcsDOUBLE deltaDec;
        if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        sprintf(separation, "%.0lf/%.0lf", deltaRa, deltaDec);
        
        geom = "&-c.geom=b&-c.bm=";
    }
    else
    {
        mcsDOUBLE radius;
        if (request.GetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        sprintf(separation, "%.0lf", radius);

        geom = "&-c.rm=";
    }

    // Add query constraints:
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, geom);
    miscDynBufAppendString(&_query, separation);        
    // TODO: define units arcmin or arcsec ??
//    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}

/*___oOo___*/
