/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_PHOTO class definition.
 * 
 * The PHOTO catalog ["II/7A/catalog"] is used in primary and secondary requests for BRIGHT scenarios 
 * to get johnson magnitudes
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
#include "vobsCATALOG_PHOTO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_PHOTO::vobsCATALOG_PHOTO() : vobsREMOTE_CATALOG(vobsCATALOG_PHOTO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE)
{
}

/*
 * Class destructor
 */
vobsCATALOG_PHOTO::~vobsCATALOG_PHOTO()
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // note: U band is not retrieved.

    // Get the johnson magnitude B (PHOT_JHN_B) stored in the 'vobsSTAR_PHOT_JHN_B' property
    miscDynBufAppendString(&_query, "&-out=B");

    // Get the johnson magnitude V (PHOT_JHN_V) stored in the 'vobsSTAR_PHOT_JHN_V' property
    miscDynBufAppendString(&_query, "&-out=V");

    // Get the johnson magnitude R (PHOT_JHN_R) stored in the 'vobsSTAR_PHOT_JHN_R' property
    miscDynBufAppendString(&_query, "&-out=R");

    // Get the johnson magnitude I (PHOT_JHN_I) stored in the 'vobsSTAR_PHOT_JHN_I' property
    miscDynBufAppendString(&_query, "&-out=I");

    // Get the johnson magnitude J (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=J");

    // Get the johnson magnitude H (PHOT_JHN_H) stored in the 'vobsSTAR_PHOT_JHN_H' property
    miscDynBufAppendString(&_query, "&-out=H");

    // Get the johnson magnitude K (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=K");

    // Get the johnson magnitude L (PHOT_JHN_L) stored in the 'vobsSTAR_PHOT_JHN_L' property
    miscDynBufAppendString(&_query, "&-out=L");

    // Get the johnson magnitude M (PHOT_JHN_M) stored in the 'vobsSTAR_PHOT_JHN_M' property
    miscDynBufAppendString(&_query, "&-out=M");

    // Get the johnson magnitude N (PHOT_IR_N:10.4) stored in the 'vobsSTAR_PHOT_JHN_N' property
    miscDynBufAppendString(&_query, "&-out=N");

    return mcsSUCCESS;
}

/*___oOo___*/
