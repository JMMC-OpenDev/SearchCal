/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_BSC class definition.
 * 
 * The BSC catalog ["V/50/catalog"] is used in secondary requests for BRIGHT scenarios 
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
#include "vobsCATALOG_BSC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_BSC::vobsCATALOG_BSC() : vobsREMOTE_CATALOG(vobsCATALOG_BSC_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_BSC::~vobsCATALOG_BSC()
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
mcsCOMPL_STAT vobsCATALOG_BSC::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios
    
    // Get the HD identifier (ID_ALTERNATIVE) stored in the 'vobsSTAR_ID_HD' property
    miscDynBufAppendString(&_query, "&-out=HD");
    
    // Get the rotational velocity RotVel (VELOC_ROTAT) stored in the 'vobsSTAR_VELOC_ROTAT' property
    miscDynBufAppendString(&_query, "&-out=RotVel");
            
    return mcsSUCCESS;
}


/*___oOo___*/
