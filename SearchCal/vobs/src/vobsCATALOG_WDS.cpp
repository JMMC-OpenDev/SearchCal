/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_WDS class.
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
#include "vobsCATALOG_WDS.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_WDS::vobsCATALOG_WDS() : vobsREMOTE_CATALOG(vobsCATALOG_WDS_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_WDS::~vobsCATALOG_WDS()
{
}

/*
 * Protected methods
 */
/**
 * Build the URL part of the CDS query that is specific to WDS.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_WDS::WriteQuerySpecificPart(void)
{
    // Get the WDS sequential index
    miscDynBufAppendString(&_query, "&-out=WDS");
    // This value will be stored in the 'vobsSTAR_ID_WDS' star property

    // Get the first mesured separation
    miscDynBufAppendString(&_query, "&-out=sep1");
    // This value will be stored in the 'ORBIT_SEPARATION_SEP1' star property

    // Get the last mesured separation
    miscDynBufAppendString(&_query, "&-out=sep2");
    // This value will be stored in the 'ORBIT_SEPARATION_SEP2' star property


    return mcsSUCCESS;
}


/*___oOo___*/
