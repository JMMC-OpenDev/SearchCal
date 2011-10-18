/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_HIC class definition.
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
#include "vobsCATALOG_HIC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_HIC::vobsCATALOG_HIC() : vobsREMOTE_CATALOG(vobsCATALOG_HIC_ID)
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_HIC::~vobsCATALOG_HIC()
{
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
mcsCOMPL_STAT vobsCATALOG_HIC::WriteQuerySpecificPart(void)
{
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LAT");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LON");
    miscDynBufAppendString(&_query, "&-out=*VELOC_HC");
    miscDynBufAppendString(&_query, "&-out=HD");
            
    return mcsSUCCESS;
}


/*___oOo___*/
