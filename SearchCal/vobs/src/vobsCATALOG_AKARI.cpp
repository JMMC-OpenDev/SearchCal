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

/*
The AKARI Infrared Astronomical Satellite observed the whole sky in
the far infrared (50-180µm) and the mid-infrared (9 and 18µm)
between May 2006 and August 2007 (Murakami et al. 2007PASJ...59S.369M)
 */

// LBO: REMOVE CLASS ASAP

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


/*___oOo___*/
