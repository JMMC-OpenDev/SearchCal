/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_LBSI.C,v 1.5 2004-11-17 07:58:07 gzins Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG_LBSI class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_LBSI.C,v 1.5 2004-11-17 07:58:07 gzins Exp $"; 
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
    miscDynBufDestroy(&_asking);
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
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_LBSI::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=Bmag,Vmag,Jmag,Hmag,Kmag");
    miscDynBufAppendString(&_asking, "&-out=UDDK,e_UDDK");
            
    return SUCCESS;
}

/*___oOo___*/
