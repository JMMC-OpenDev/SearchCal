/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * vobsCATALOG_BSC class definition.
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
#include "vobsCATALOG_BSC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_BSC::vobsCATALOG_BSC() : vobsREMOTE_CATALOG("V/50/catalog")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_BSC::~vobsCATALOG_BSC()
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_BSC::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_BSC::GetAskingSpecificParameters()");
  
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*VELOC_ROTAT");
    miscDynBufAppendString(&_query, "&-out=HD");
            
    return mcsSUCCESS;
}



/*___oOo___*/
