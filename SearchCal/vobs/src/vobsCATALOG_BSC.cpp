/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_BSC.cpp,v 1.4 2005-02-08 20:30:37 gzins Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.3  2005/01/26 08:11:28  scetre
* change history
*
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG_BSC class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_BSC.cpp,v 1.4 2005-02-08 20:30:37 gzins Exp $"; 
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
#include "vobsCATALOG_BSC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_BSC::vobsCATALOG_BSC()
{
    SetName("V/50/catalog");
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_BSC::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_BSC::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*VELOC_ROTAT&-out=HD");
            
    return mcsSUCCESS;
}



/*___oOo___*/
