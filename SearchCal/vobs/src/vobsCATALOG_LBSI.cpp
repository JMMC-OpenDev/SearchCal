/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_LBSI class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_LBSI.cpp,v 1.17 2011-03-03 13:09:42 lafrasse Exp $"; 

/* 
 * System Headers 
 */
#include <iostream>
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

/**
 * Build a catalog object.
 */
vobsCATALOG_LBSI::vobsCATALOG_LBSI() : 
                                    vobsREMOTE_CATALOG("J/A+A/393/183/catalog")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_LBSI::~vobsCATALOG_LBSI()
{
    miscDynBufDestroy(&_query);
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
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_LBSI::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=Lmag");
    miscDynBufAppendString(&_query, "&-out=Mmag");
    miscDynBufAppendString(&_query, "&-out=Nmag");
    miscDynBufAppendString(&_query, "&-out=UDDK");
    miscDynBufAppendString(&_query, "&-out=e_UDDK");
            
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
 * 
 */
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_LBSI::GetAskingSpecificParameters()");

    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange;
    mcsDOUBLE maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);

    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.eq=J2000");
//    miscDynBufAppendString(&_query, "&-out.max=100");
    miscDynBufAppendString(&_query, "&-out.max=1000");
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=Lmag");
    miscDynBufAppendString(&_query, "&-out=Mmag");
    miscDynBufAppendString(&_query, "&-out=Nmag");
    miscDynBufAppendString(&_query, "&-out=UDDK");
    miscDynBufAppendString(&_query, "&-out=e_UDDK");
    
    return mcsSUCCESS;
}

/*___oOo___*/
