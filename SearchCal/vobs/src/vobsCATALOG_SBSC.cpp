/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_SBSC.cpp,v 1.2 2005-01-24 10:58:44 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_SBSC class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_SBSC.cpp,v 1.2 2005-01-24 10:58:44 scetre Exp $"; 
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
#include "vobsCATALOG_SBSC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_SBSC::vobsCATALOG_SBSC()
{
    strcpy(_name,"V/36B/bsc4s");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_SBSC::~vobsCATALOG_SBSC()
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
mcsCOMPL_STAT vobsCATALOG_SBSC::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_SBSC::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*VELOC_ROTAT");
            
    return mcsSUCCESS;
}

/*___oOo___*/
