/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_AKARI class.
 * 
 * The AKARI catalog ["II/297/irc"] is used in all secondary requests
 * to get IR fluxes at 9 and 18 mu
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
#include "vobsCATALOG_AKARI.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_AKARI::vobsCATALOG_AKARI() : vobsREMOTE_CATALOG(vobsCATALOG_AKARI_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_AKARI::~vobsCATALOG_AKARI()
{
}

/*
 * Private methods
 */

/**
 * Build the URL part of the CDS query that is specific to AKARI.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_AKARI::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates
    
    // Get the AKARI sequential index objID (ID_NUMBER) stored in the 'vobsSTAR_ID_AKARI' property
    miscDynBufAppendString(&_query, "&-out=objID");

    // Get the flux density S09 (PHOT_FLUX_IR_9) stored in the 'vobsSTAR_PHOT_FLUX_IR_09' property
    miscDynBufAppendString(&_query, "&-out=S09");

    // Get the flux density error e_S09 (ERROR) stored in the 'vobsSTAR_PHOT_FLUX_IR_09_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_S09");

    // Get the flux density S18 (wrong ucd PHOT_FLUX_IR_25) stored in the 'vobsSTAR_PHOT_FLUX_IR_18' property
    miscDynBufAppendString(&_query, "&-out=S18"); 

    // Get the flux density error e_S18 (ERROR) stored in the 'vobsSTAR_PHOT_FLUX_IR_18_ERROR' property
    miscDynBufAppendString(&_query, "&-out=e_S18"); 

    return mcsSUCCESS;
}


/*___oOo___*/
