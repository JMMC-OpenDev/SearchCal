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

// LBO: REMOVE CLASS ASAP

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

/*___oOo___*/
