/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_LBSI class definition.
 * 
 * The LBSI catalog ["J/A+A/393/183/catalog"] is used in primary and secondary requests for BRIGHT K and V scenarios 
 * to get magnitudes and UDDK diameter
 * 
 * J/A+A/393/183/catalog
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
#include "vobsCATALOG_LBSI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_LBSI::vobsCATALOG_LBSI() : vobsREMOTE_CATALOG(vobsCATALOG_LBSI_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_LBSI::~vobsCATALOG_LBSI()
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
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT K and V scenarios

    // Get the johnson magnitude Bmag (PHOT_JHN_B) stored in the 'vobsSTAR_PHOT_JHN_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");

    // Get the johnson magnitude Vmag (PHOT_JHN_V) stored in the 'vobsSTAR_PHOT_JHN_V' property
    miscDynBufAppendString(&_query, "&-out=Vmag");

    // Get the johnson magnitude Jmag (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=Jmag");

    // Get the johnson magnitude Hmag (PHOT_JHN_H) stored in the 'vobsSTAR_PHOT_JHN_H' property
    miscDynBufAppendString(&_query, "&-out=Hmag");

    // Get the johnson magnitude Kmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Kmag");

    // Get the johnson magnitude Lmag (PHOT_JHN_L) stored in the 'vobsSTAR_PHOT_JHN_L' property
    miscDynBufAppendString(&_query, "&-out=Lmag");

    // Get the johnson magnitude Mmag (PHOT_JHN_M) stored in the 'vobsSTAR_PHOT_JHN_M' property
    miscDynBufAppendString(&_query, "&-out=Mmag");

    // TODO: why not get the magitudes in N band as descibed in doc JMMC-MEM-2600-0004 ?

    // Get the uniform disk diameter UDDK (EXTENSION_DIAM) stored in the 'vobsSTAR_UDDK_DIAM' property
    miscDynBufAppendString(&_query, "&-out=UDDK");

    // Get the uniform disk diameter error e_UDDK (ERROR) stored in the 'vobsSTAR_UDDK_DIAM_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_UDDK");

    return mcsSUCCESS;
}

/*___oOo___*/
