/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_HIC class definition.
 * 
 * The HIC catalog ["I/196/main"] is used in secondary requests for BRIGHT scenarios 
 * to get radial velocity
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
#include "vobsCATALOG_HIC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_HIC::vobsCATALOG_HIC() : vobsREMOTE_CATALOG(vobsCATALOG_HIC_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_HIC::~vobsCATALOG_HIC()
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
mcsCOMPL_STAT vobsCATALOG_HIC::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // Get the HD identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_HD' property
    miscDynBufAppendString(&_query, "&-out=HD");

    // Get the radial velocity RV (VELOC_HC) stored in the 'vobsSTAR_VELOC_HC' property
    miscDynBufAppendString(&_query, "&-out=RV");

    return mcsSUCCESS;
}


/*___oOo___*/
