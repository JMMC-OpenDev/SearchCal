/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_MERAND class.
 * 
 * The MERAND catalog ["J/A+A/433/1155"] is used in primary and secondary requests for BRIGHT K and V scenarios 
 * to get magnitudes and UDDK diameter
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
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(void)
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

    // Get the johnson magnitude Ksmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Ksmag");

    // Get the uniform disk diameter UDdiamKs (EXTENSION_DIAM) stored in the 'vobsSTAR_UDDK_DIAM' property
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");

    // Get the uniform disk diameter error e_UDdiam (ERROR) stored in the 'vobsSTAR_UDDK_DIAM_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");

    return mcsSUCCESS;
}

/*___oOo___*/
