/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_SBSC class definition.
 * 
 * The SBSC catalog ["V/36B/bsc4s"] is used in secondary requests for BRIGHT scenarios 
 * to get rotational velocity
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
#include "misc.h"

/*
 * Local Headers 
 */
#include "vobsCATALOG_SBSC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_SBSC::vobsCATALOG_SBSC() : vobsREMOTE_CATALOG(vobsCATALOG_SBSC_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_SBSC::~vobsCATALOG_SBSC()
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
mcsCOMPL_STAT vobsCATALOG_SBSC::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // Get the HD identifier (ID_MAIN) stored in the 'vobsSTAR_ID_HD' property
    miscDynBufAppendString(&_query, "&-out=HD");

    // Get the rotational velocity vsini (VELOC_ROTAT) stored in the 'vobsSTAR_VELOC_ROTAT' property
    miscDynBufAppendString(&_query, "&-out=vsini");
    
    // TODO: add condition on vsini not empty ...

    return mcsSUCCESS;
}


/*___oOo___*/
