/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_DENIS class definition.
 * 
 * The DENIS catalog ["B/denis"] is used in secondary requests for the FAINT scenario
 * to get cousin I magnitude, photometric R and B magnitudes and USNO coordinates
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
#include "vobsCATALOG_DENIS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_ID)
{
}

/*
 * Class destructor
 */
vobsCATALOG_DENIS::~vobsCATALOG_DENIS()
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
mcsCOMPL_STAT vobsCATALOG_DENIS::WriteQuerySpecificPart(void)
{
    // SECONDARY REQUEST: cone search arround given star coordinates for the FAINT scenario 
    
    // Get the identifier DENIS (ID_MAIN) stored in the 'vobsSTAR_ID_DENIS' property
    miscDynBufAppendString(&_query, "&-out=DENIS");    

    // A2RAdeg / A2DEdeg = USNOA2.0 nearest match: TODO what use = query UNSO catalog ?
    
    // Get the RA  USNOA2.0 nearest match A2RAdeg (POS_EQ_RA_OTHER)  stored in the 'vobsSTAR_POS_EQ_RA_OTHER' property
    miscDynBufAppendString(&_query, "&-out=A2RAdeg");
    
    // Get the DEC USNOA2.0 nearest match A2DEdeg (POS_EQ_DEC_OTHER) stored in the 'vobsSTAR_POS_EQ_DEC_OTHER' property
    miscDynBufAppendString(&_query, "&-out=A2DEdeg");

    // Get the cousin magnitude Imag at 0.82 mu (PHOT_COUS_I) stored in the 'vobsSTAR_PHOT_COUS_I' property
    miscDynBufAppendString(&_query, "&-out=Imag");

    // Get the quality flag Iflg (CODE_MISC) stored in the 'vobsSTAR_CODE_MISC_I' property
    miscDynBufAppendString(&_query, "&-out=Iflg");
    
    // Get the photometric magnitude Rmag (PHOT_PHG_R) stored in the 'vobsSTAR_PHOT_PHG_R' property
    miscDynBufAppendString(&_query, "&-out=Rmag");
    
    // Get the photometric magnitude Bmag (PHOT_PHG_B) stored in the 'vobsSTAR_PHOT_PHG_B' property
    miscDynBufAppendString(&_query, "&-out=Bmag");
            
    return mcsSUCCESS;
}


/*___oOo___*/
