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
    miscDynBufDestroy(&_query);
}

/*
 * Protected methods
 */

/**
 * Build the constant part of the asking for CIO catalog
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * @return always mcsSUCCESS.
 *
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQueryConstantPart(void)
{
    logTrace("vobsCATALOG_CIO::GetAskingConstant()");

    miscDynBufAppendString(&_query,"&-file=-c");
    miscDynBufAppendString(&_query, "&-c.eq=J2000");
//    miscDynBufAppendString(&_query,"&-out.max=50");
    miscDynBufAppendString(&_query,"&-out.max=1000");
    miscDynBufAppendString(&_query,"-c.r=1");
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");

    return mcsSUCCESS;
}



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
    logTrace("vobsCATALOG_CIO::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    miscDynBufAppendString(&_query, "&-out=x_F(IR)");
    //constraints
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query,"&lambda=1.25,1.65,2.20,3.5,5.0,10.0");
    
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
    logTrace("vobsCATALOG_CIO::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&F(IR)=");
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange;
    mcsDOUBLE maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    miscDynBufAppendString(&_query, rangeMag);
    
    miscDynBufAppendString(&_query, "&lambda=");


    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
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

    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);
//    miscDynBufAppendString(&_query, "&-out.max=50");
    miscDynBufAppendString(&_query, "&-out.max=1000");
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    miscDynBufAppendString(&_query, "&-out=x_F(IR)");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}


/*___oOo___*/
