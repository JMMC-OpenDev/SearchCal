/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_DENIS_JK class.
 * 
 * The DENIS_JK catalog ["J/A+A/413/1037/table1"] is used in secondary requests for BRIGHT scenarios 
 * to get johnson J and K magnitudes
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
#include "vobsCATALOG_DENIS_JK.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_DENIS_JK::vobsCATALOG_DENIS_JK() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_JK_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_DENIS_JK::~vobsCATALOG_DENIS_JK()
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
mcsCOMPL_STAT vobsCATALOG_DENIS_JK::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for BRIGHT scenarios

    // Get the johnson magnitude Jmag (PHOT_JHN_J) stored in the 'vobsSTAR_PHOT_JHN_J' property
    miscDynBufAppendString(&_query, "&-out=Jmag");

    // Get the johnson magnitude Ksmag (PHOT_JHN_K) stored in the 'vobsSTAR_PHOT_JHN_K' property
    miscDynBufAppendString(&_query, "&-out=Ksmag");

    // Get the variability index Var (CODE_VARIAB) stored in the 'vobsSTAR_CODE_BIN_FLAG' property
    miscDynBufAppendString(&_query, "&-out=Var");

    // Add variability constraint: Var < 4
    miscDynBufAppendString(&_query, "&Var=%3C4");

    return mcsSUCCESS;
}


/*___oOo___*/
