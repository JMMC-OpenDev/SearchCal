/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_CIO class definition.
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
#include "vobsCATALOG_CIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_CIO::vobsCATALOG_CIO() : vobsREMOTE_CATALOG(vobsCATALOG_CIO_ID)
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_CIO::~vobsCATALOG_CIO()
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
 * @return always mcsSUCCESS.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(void)
{
    // constraints
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&lambda=1.25,1.65,2.20,3.5,5.0,10.0");

    // properties to retrieve
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    
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
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(vobsREQUEST &request)
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
    miscDynBufAppendString(&_query, "&lambda=");
    if (band[0] =='K')
    {
        miscDynBufAppendString(&_query, "2.20");
    }
    else if (band[0] == 'H')
    {
        miscDynBufAppendString(&_query, "1.65");        
    }
    else if (band[0] == 'J')
    {
        miscDynBufAppendString(&_query, "1.25");        
    }
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
        
    miscDynBufAppendString(&_query, "&F(IR)=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    // TODO: why arcmin and not arcsec ??
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    
    // order by distance
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}


/*___oOo___*/
