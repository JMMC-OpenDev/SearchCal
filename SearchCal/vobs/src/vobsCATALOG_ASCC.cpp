/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsCATALOG_ASCC class.
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
#include "vobsCATALOG_ASCC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_ASCC::vobsCATALOG_ASCC() : vobsREMOTE_CATALOG(vobsCATALOG_ASCC_ID)
{
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_ASCC::~vobsCATALOG_ASCC()
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_ASCC::GetAskingSpecificParameters()");
    
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=e_Plx");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
//     miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
//     miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_H");
//     miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=v1");
    miscDynBufAppendString(&_query, "&-out=v2");
    miscDynBufAppendString(&_query, "&-out=v3");
    miscDynBufAppendString(&_query, "&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP");
    miscDynBufAppendString(&_query, "&-out=HD");
    miscDynBufAppendString(&_query, "&-out=DM");
    miscDynBufAppendString(&_query, "&-out=TYC1");
    miscDynBufAppendString(&_query, "&-out=TYC2");
    miscDynBufAppendString(&_query, "&-out=TYC3");
    // constraints
    
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
 * 
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_ASCC::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&");
    
    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    miscDynBufAppendString(&_query, band);
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange;
    mcsDOUBLE maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2lf..%.2lf", minMagRange, maxMagRange);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);

    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0lf/%.0lf", deltaRa, deltaDec);

    // miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=100&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=1000&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    // @TODO : Will get galactic coordinates from ASCC instead of 2MASS
    // miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000,_Glon,_Glat&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=e_Plx");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    // @TODO : Will replace 2MASS querying
    // miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    // miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_H");
    // miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=v1");
    miscDynBufAppendString(&_query, "&-out=v2");
    miscDynBufAppendString(&_query, "&-out=v3");
    miscDynBufAppendString(&_query, "&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP");
    miscDynBufAppendString(&_query, "&-out=HD");
    miscDynBufAppendString(&_query, "&-out=DM");
    miscDynBufAppendString(&_query, "&-out=TYC1");
    // constraints
    miscDynBufAppendString(&_query, "&-sort=_r"); 
    
    return mcsSUCCESS;
}

/*___oOo___*/
