/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsCATALOG_UNSO class.
 * 
 * The UNSO catalog ["I/284"] is used in secondary requests for the FAINT scenario 
 * to get proper motions and magnitudes
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
#include "vobsCATALOG_UNSO.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_UNSO::vobsCATALOG_UNSO() : vobsREMOTE_CATALOG(vobsCATALOG_UNSO_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_UNSO::~vobsCATALOG_UNSO()
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
mcsCOMPL_STAT vobsCATALOG_UNSO::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for the FAINT scenario 

    // Get the proper motion pmRA (POS_EQ_PMRA) stored in the 'vobsSTAR_POS_EQ_PMRA' property
    miscDynBufAppendString(&_query, "&-out=pmRA");
    
    // Get the proper motion pmDE (POS_EQ_PMDEC) stored in the 'vobsSTAR_POS_EQ_PMDEC' property
    miscDynBufAppendString(&_query, "&-out=pmDE");

    // Get the photometric magnitude Imag (PHOT_PHG_I) stored in the 'vobsSTAR_PHOT_PHG_I' property
    miscDynBufAppendString(&_query, "&-out=Imag");

    // 2 photometric Rmag/Bmag are available, TODO: decide/validate which magnitudes use

    // Get the photometric magnitude R1mag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    miscDynBufAppendString(&_query, "&-out=R1mag");

    // Get the photometric magnitude R2mag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    miscDynBufAppendString(&_query, "&-out=R2mag");
    
    // Get the photometric magnitude B1mag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    miscDynBufAppendString(&_query, "&-out=B1mag");

    // Get the photometric magnitude B2mag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    miscDynBufAppendString(&_query, "&-out=B2mag");
    
    return mcsSUCCESS;
}


/*___oOo___*/
