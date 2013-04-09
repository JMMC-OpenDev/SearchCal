/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_CIO class definition.
 * 
 * The CIO catalog ["II/225/catalog"] is used in secondary requests for BRIGHT scenarios 
 * to get IR johnson magnitudes
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
#include "vobsCATALOG_CIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

// LBO: REMOVE CLASS ASAP

/*
 * Class constructor
 */
vobsCATALOG_CIO::vobsCATALOG_CIO() : vobsREMOTE_CATALOG(vobsCATALOG_CIO_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_CIO::~vobsCATALOG_CIO()
{
}

/*___oOo___*/
