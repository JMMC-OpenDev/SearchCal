/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition vobsCATALOG_ASCC class.
 * 
 * The ASCC catalog ["I/280"] is used in primary + secondary requests for BRIGHT scenarios 
 * and in the secondary request for FAINT scenario
 * to get many identifiers, B and V magnitudes, parallax, proper motions and variability flags
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
#include "vobsCATALOG_ASCC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_ASCC::vobsCATALOG_ASCC() : vobsREMOTE_CATALOG(vobsCATALOG_ASCC_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_ASCC::~vobsCATALOG_ASCC()
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT and FAINT scenarios
    
    // Get the HIP identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_HIP' property
    miscDynBufAppendString(&_query, "&-out=HIP");
    
    // Get the HD identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_HD' property
    miscDynBufAppendString(&_query, "&-out=HD");
    
    // Get the DM identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_DM' property
    miscDynBufAppendString(&_query, "&-out=DM");
    
    // Get the TYC1 identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_TYC1' property
    miscDynBufAppendString(&_query, "&-out=TYC1");
    
    // Get the TYC2 identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_TYC2' property
    miscDynBufAppendString(&_query, "&-out=TYC2");
    
    // Get the TYC3 identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_TYC3' property
    miscDynBufAppendString(&_query, "&-out=TYC3");

    // Get the proper motion pmRA (POS_EQ_PMRA) stored in the 'vobsSTAR_POS_EQ_PMRA' property
    miscDynBufAppendString(&_query, "&-out=pmRA");
    
    // Get the proper motion pmDE (POS_EQ_PMDEC) stored in the 'vobsSTAR_POS_EQ_PMDEC' property
    miscDynBufAppendString(&_query, "&-out=pmDE");
    
    // Get the parallax Plx (POS_PARLX_TRIG) stored in the 'vobsSTAR_POS_PARLX_TRIG' property
    miscDynBufAppendString(&_query, "&-out=Plx");

    // Get the parallax error e_Plx (ERROR) stored in the 'vobsSTAR_POS_PARLX_TRIG_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_Plx");
    
    // Get the spectral type SpType (SPECT_TYPE_MK) stored in the 'vobsSTAR_SPECT_TYPE_MK' property
    miscDynBufAppendString(&_query, "&-out=SpType");

    // Get the johnson magnitude Bmag (PHOT_JHN_B) stored in the 'vobsSTAR_PHOT_JHN_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");
    
    // Get the johnson magnitude Vmag (PHOT_JHN_V) stored in the 'vobsSTAR_PHOT_JHN_V' property
    miscDynBufAppendString(&_query, "&-out=Vmag");

    // Get the variability v1 (CODE_VARIAB) stored in the 'vobsSTAR_CODE_VARIAB_V1' property
    miscDynBufAppendString(&_query, "&-out=v1");

    // Get the variability v2 (CODE_VARIAB) stored in the 'vobsSTAR_CODE_VARIAB_V2' property
    miscDynBufAppendString(&_query, "&-out=v2");

    // Get the variability v3 (VAR_CLASS) stored in the 'vobsSTAR_CODE_VARIAB_V3' property
    miscDynBufAppendString(&_query, "&-out=v3");

    // Get the multiplicty flag d5 (CODE_MULT_FLAG) stored in the 'vobsSTAR_CODE_MULT_FLAG' property
    miscDynBufAppendString(&_query, "&-out=d5");
    
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
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(vobsREQUEST &request)
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
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm="); // -c.bm means box in arcmin
    miscDynBufAppendString(&_query, separation);
    
    // properties to retrieve
    return WriteQuerySpecificPart();
}


/*___oOo___*/
