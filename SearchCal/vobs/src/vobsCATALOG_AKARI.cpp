/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_AKARI class.
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
 * Protected methods
 */
/**
 * Build the URL part of the CDS query that is specific to AKARI.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_AKARI::WriteQuerySpecificPart(void)
{
    // Get the AKARI sequential index
    miscDynBufAppendString(&_query, "&-out=objID");
    // This value will be stored in the 'vobsSTAR_ID_AKARI' star property

    // Get the Flux density in filter S9W
    miscDynBufAppendString(&_query, "&-out=S09");
    // This value will be stored in the 'vobsSTAR_PHOT_FLUX_IR_09' star property

    // Get the Flux density Error in filter S9W
    miscDynBufAppendString(&_query, "&-out=e_S09");
    // This value will be stored in the 'vobsSTAR_PHOT_FLUX_IR_09_ERROR' star property

    // Get the Flux density in filter L18W
    miscDynBufAppendString(&_query, "&-out=S18"); 
    // This value will be stored in the 'vobsSTAR_PHOT_FLUX_IR_18' star property

    // Get the Flux density Error in filter L18W
    miscDynBufAppendString(&_query, "&-out=e_S18"); 
    // This value will be stored in the 'vobsSTAR_PHOT_FLUX_IR_18_ERROR' star property


    return mcsSUCCESS;
}


/*___oOo___*/
