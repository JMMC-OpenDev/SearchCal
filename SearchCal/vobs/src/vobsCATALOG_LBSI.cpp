/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_LBSI.cpp,v 1.3 2005-01-26 08:11:28 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG_LBSI class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_LBSI.cpp,v 1.3 2005-01-26 08:11:28 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
#include "vobsCATALOG_LBSI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_LBSI::vobsCATALOG_LBSI()
{
    strcpy(_name,"J/A+A/393/183/catalog");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_LBSI::~vobsCATALOG_LBSI()
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_LBSI::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=Bmag,Vmag,Jmag,Hmag,Kmag");
    miscDynBufAppendString(&_query, "&-out=UDDK,e_UDDK");
            
    return mcsSUCCESS;
}

/*___oOo___*/
