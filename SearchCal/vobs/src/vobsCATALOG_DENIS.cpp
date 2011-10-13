/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_DENIS class definition.
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

/**
 * Build a catalog object.
 */
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG(vobsCATALOG_DENIS_ID)
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_DENIS::~vobsCATALOG_DENIS()
{
    miscDynBufDestroy(&_query);
}


/*
 * Protected methods
 */

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 *
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_DENIS::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_DENIS::GetAskingSpecificParameters()");
   
    // A2RAdeg (POS_EQ_RA_OTHER) / A2DEdeg (POS_EQ_DEC_OTHER)
    // =USNOA2.0 nearest match: useful to query UNSO catalog ?
    miscDynBufAppendString(&_query, "&-out=A2RAdeg");
    miscDynBufAppendString(&_query, "&-out=A2DEdeg");

    // Imag at 0.82 mu (PHOT_COUS_I)
    miscDynBufAppendString(&_query, "&-out=Imag");
    // Rmag (PHOT_PHG_R)
    miscDynBufAppendString(&_query, "&-out=Rmag");
    // Bmag (PHOT_PHG_B)
    miscDynBufAppendString(&_query, "&-out=Bmag");
    // Iflg (CODE_MISC)
    miscDynBufAppendString(&_query, "&-out=Iflg");
    // DENIS (ID_MAIN)
    miscDynBufAppendString(&_query, "&-out=DENIS");
            
    return mcsSUCCESS;
}



/*___oOo___*/
