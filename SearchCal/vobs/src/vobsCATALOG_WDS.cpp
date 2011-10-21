/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_WDS class.
 * 
 * The WDS catalog ["B/wds/wds"] is used in all secondary requests 
 * to detect multiple stars (separation < 2 arc sec)
 */


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

/*
 * Local Headers 
 */
#include "vobsCATALOG_WDS.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 * 
 * note: Do not sort results because WDS has multiple records for the same RA/DEC coordinates
 */
vobsCATALOG_WDS::vobsCATALOG_WDS() : vobsREMOTE_CATALOG(vobsCATALOG_WDS_ID, false)
{
}

/**
 * Class destructor
 */
vobsCATALOG_WDS::~vobsCATALOG_WDS()
{
}


/*
 * Private methods
 */

/**
 * Build the URL part of the CDS query that is specific to WDS.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_WDS::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates
    
    // Get the WDS sequential index WDS (ID_MAIN) stored in the 'vobsSTAR_ID_WDS' property
    miscDynBufAppendString(&_query, "&-out=WDS");

    // Get the first mesured separation sep1 (ORBIT_SEPARATION) stored in the 'ORBIT_SEPARATION_SEP1' property
    miscDynBufAppendString(&_query, "&-out=sep1");

    // Get the last mesured separation sep2 (ORBIT_SEPARATION) stored in the 'ORBIT_SEPARATION_SEP2' property
    miscDynBufAppendString(&_query, "&-out=sep2");

    return mcsSUCCESS;
}


/*___oOo___*/
