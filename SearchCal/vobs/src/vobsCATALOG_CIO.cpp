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

/*
 * Class constructor
 */
vobsCATALOG_CIO::vobsCATALOG_CIO() : vobsREMOTE_CATALOG(vobsCATALOG_CIO_ID, 1.0, EPOCH_2000, EPOCH_2000, mcsFALSE, mcsTRUE)
{
}

/*
 * Class destructor
 */
vobsCATALOG_CIO::~vobsCATALOG_CIO()
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
 * @return always mcsSUCCESS.
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // note: following properties are managed using specific code in vobsCDATA.h

    // Get the wavelength lambda (INST_WAVELENGTH_VALUE) not stored in any star property
    miscDynBufAppendString(&_query, "&-out=lambda");

    // Get the IR magnitude (PHOT_FLUX_IR_MISC) not stored in any star property
    miscDynBufAppendString(&_query, "&-out=F(IR)");

    // Get magnitudes for given bands (J, H, K, L, M, N) stored in the 'vobsSTAR_PHOT_JHN_?' property
    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&lambda=1.25,1.65,2.20,3.5,5.0,10.0");

    return mcsSUCCESS;
}

/*___oOo___*/
