/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_SB9 class.
 * 
 * The SB9 catalog ["B/sb9/main"] is used in all secondary requests 
 * to detect spectral binary stars
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
#include "vobsCATALOG_SB9.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_SB9::vobsCATALOG_SB9() : vobsREMOTE_CATALOG(vobsCATALOG_SB9_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_SB9::~vobsCATALOG_SB9()
{
}


/*
 * Private methods
 */

/**
 * Build the URL part of the CDS query that is specific to SB9.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 */
mcsCOMPL_STAT vobsCATALOG_SB9::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates

    // Get the SB9 sequential index Seq (ID_MAIN) stored in the 'vobsSTAR_ID_SB9' property
    miscDynBufAppendString(&_query, "&-out=Seq");

    return mcsSUCCESS;
}


/*___oOo___*/
