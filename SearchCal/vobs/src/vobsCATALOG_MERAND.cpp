/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_MERAND class.
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
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(void)
{
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");
            
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
    // TODO: why arcmin and not arcsec ??
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    
    // properties to retrieve
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");
    
    return mcsSUCCESS;
}


/*___oOo___*/
