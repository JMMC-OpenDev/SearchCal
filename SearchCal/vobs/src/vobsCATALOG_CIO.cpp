/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_CIO class definition.
 * 
 * The CIO catalog ["II/225/catalog"] is used in secondary requests for BRIGHT scenarios 
 * to get IR johnson magnitudes
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
#include "vobsCATALOG_CIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_CIO::vobsCATALOG_CIO() : vobsREMOTE_CATALOG(vobsCATALOG_CIO_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_CIO::~vobsCATALOG_CIO()
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
 * @return always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios
    
    // note: following properties are managed using specific code in vobsCDATA.h
        
    // Get the wavelength lambda (INST_WAVELENGTH_VALUE) not stored in any star property
    miscDynBufAppendString(&_query, "&-out=lambda");

    // Get the IR magnitude (PHOT_FLUX_IR_MISC) not stored in any star property
    miscDynBufAppendString(&_query, "&-out=F(IR)");

    // constraints: get magnitudes for given bands (J, H, K, L, M, N)
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&lambda=1.25,1.65,2.20,3.5,5.0,10.0");
            
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
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(vobsREQUEST &request)
{
    // note: CIO used in PRIMARY request in BRIGHT K scenario (I, J, H, K)
    
    
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
    if (band[0] == 'K')
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
    else if (band[0] == 'I')
    {
        // TODO: magnitude I is not supposed to be present in this catalog: how to query on I mag ???
        miscDynBufAppendString(&_query, "0.0");        
    }
    
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
        
    miscDynBufAppendString(&_query, "&F(IR)=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm="); // -c.bm means box in arcmin
    miscDynBufAppendString(&_query, separation);
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
