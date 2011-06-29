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
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG("B/denis")
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
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_RA_OTHER");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_DEC_OTHER");
    miscDynBufAppendString(&_query, "&-out=*PHOT_COUS_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_B");
    miscDynBufAppendString(&_query, "&-out=Iflg");
    miscDynBufAppendString(&_query, "&-out=DENIS");
            
    return mcsSUCCESS;
}



/*___oOo___*/
