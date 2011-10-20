/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_MASS class definition.
 * 
 * The 2MASS catalog ["II/246/out"] is used in primary request for FAINT scenario and in secondary requests for BRIGHT scenarios 
 * to get galactic coordinates and many magnitudes
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
vobsCATALOG_MASS::vobsCATALOG_MASS() : vobsREMOTE_CATALOG(vobsCATALOG_MASS_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_MASS::~vobsCATALOG_MASS()
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
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios
    
    // Get the identifier 2MASS (ID_MAIN) stored in the 'vobsSTAR_ID_2MASS' property
    miscDynBufAppendString(&_query, "&-out=2MASS");    
    
    // Get the galactic latitude  GLAT (POS_GAL_LAT) stored in the 'vobsSTAR_POS_GAL_LAT' property
    miscDynBufAppendString(&_query, "&-out=GLAT");

    // Get the galactic longitude GLON (POS_GAL_LON) stored in the 'vobsSTAR_POS_GAL_LON' property
    miscDynBufAppendString(&_query, "&-out=GLON");
    
    // Get the johnson magnitude Jmag (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=Jmag");
    
    // Get the johnson magnitude Hmag (PHOT_JHN_H) stored in the 'vobsSTAR_PHOT_JHN_H' property
    miscDynBufAppendString(&_query, "&-out=Hmag");
    
    // Get the johnson magnitude Kmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Kmag");

    // Get the photometric magnitude Rmag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    miscDynBufAppendString(&_query, "&-out=Rmag");
    
    // Get the photometric magnitude Bmag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");
    
    // Get the associated optical source opt (ID_CATALOG) stored in the 'vobsSTAR_ID_CATALOG' property
    miscDynBufAppendString(&_query, "&-out=opt");
    
    // Get the quality flag Qflg (CODE_QUALITY) stored in the 'vobsSTAR_CODE_QUALITY' property
    miscDynBufAppendString(&_query, "&-out=Qflg");
    
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

    const char* geomParam;
    mcsSTRING32 separation;
    
    // Add search geometry constraints:
    if (request.GetSearchAreaGeometry() == vobsBOX)
    {
        geomParam = "&-c.geom=b&-c.bm="; // -c.bm means box in arcmin
        
        mcsDOUBLE deltaRa;
        mcsDOUBLE deltaDec;
        if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        sprintf(separation, "%.0lf/%.0lf", deltaRa, deltaDec);
    }
    else
    {
        geomParam = "&-c.rm="; // -c.rm means radius in arcmin
        
        mcsDOUBLE radius;
        if (request.GetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        sprintf(separation, "%.0lf", radius);
    }

    // Add query constraints:
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, geomParam);
    miscDynBufAppendString(&_query, separation);        
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
