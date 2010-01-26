/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_WDS.cpp,v 1.1 2010-01-26 14:15:38 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_WDS class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: vobsCATALOG_WDS.cpp,v 1.1 2010-01-26 14:15:38 lafrasse Exp $"; 

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
    miscDynBufDestroy(&_query);    
}

/*
 * Public methods
 */


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
    logTrace("vobsCATALOG_WDS::WriteQuerySpecificPart()");

    // We want to get the WDS sequential index...
    miscDynBufAppendString(&_query, "&-out=WDS");
    // ...for each given star with sep2 <= 2 arcsec.
    miscDynBufAppendString(&_query, "&sep2=%3C2");

    // This value will be stored in the 'vobsSTAR_ID_WDS' star property

    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
