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
vobsCATALOG_ASCC::vobsCATALOG_ASCC() : vobsREMOTE_CATALOG(vobsCATALOG_ASCC_ID, 1.0, 1991.25, 1991.25, mcsTRUE)
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

    // Get the error e_RAJ2000 (ERROR) stored in the 'vobsSTAR_POS_EQ_RA_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_RAJ2000");

    // Get the error e_DEJ2000 (ERROR) stored in the 'vobsSTAR_POS_EQ_DEC_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_DEJ2000");

    // Get the proper motion pmRA (POS_EQ_PMRA) stored in the 'vobsSTAR_POS_EQ_PMRA' property
    miscDynBufAppendString(&_query, "&-out=pmRA");

    // Get the proper motion pmDE (POS_EQ_PMDEC) stored in the 'vobsSTAR_POS_EQ_PMDEC' property
    miscDynBufAppendString(&_query, "&-out=pmDE");

    // Get the error e_pmRA (ERROR) stored in the 'vobsSTAR_POS_EQ_PMRA_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_pmRA");

    // Get the error e_pmDE (ERROR) stored in the 'vobsSTAR_POS_EQ_PMDEC_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_pmDE");

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

/*___oOo___*/
