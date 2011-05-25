/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_SB9 class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: vobsCATALOG_SB9.cpp,v 1.2 2009-02-26 10:43:07 lafrasse Exp $"; 

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
    miscDynBufDestroy(&_query);    
}

/*
 * Public methods
 */


/*
 * Protected methods
 */
/**
 * Build the URL part of the CDS query that is specific to SB9.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_SB9::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_SB9::WriteQuerySpecificPart()");

    // We want to get the SB9 sequential index for each given star
    miscDynBufAppendString(&_query, "&-out=Seq");
    // This value will be stored in the 'vobsSTAR_ID_SB9' star property

    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
