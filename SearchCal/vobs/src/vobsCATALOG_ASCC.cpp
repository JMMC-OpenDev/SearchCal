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

// LBO: REMOVE CLASS ASAP

/*
 * Class constructor
 */
vobsCATALOG_ASCC::vobsCATALOG_ASCC() : vobsREMOTE_CATALOG(vobsCATALOG_ASCC_ID)
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

/*___oOo___*/
