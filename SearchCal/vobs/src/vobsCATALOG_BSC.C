/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_BSC.C,v 1.1 2004-07-28 14:18:05 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG_BSC class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_BSC.C,v 1.1 2004-07-28 14:18:05 scetre Exp $"; 
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
    strcpy(_name,"V/50/catalog");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_BSC::~vobsCATALOG_BSC()
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_BSC::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_BSC::GetAskingSpecificParameters()");
   
    miscDynStrAppendString(&_asking, "&-out=*VELOC_ROTAT&-out=HD");
            
    return SUCCESS;
}



/*___oOo___*/
