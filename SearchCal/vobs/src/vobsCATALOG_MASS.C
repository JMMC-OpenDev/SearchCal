/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_MASS.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_MASS class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MASS.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"; 
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
#include "vobsCATALOG_MASS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_MASS::vobsCATALOG_MASS()
{
    strcpy(_name,"II/246/out");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_MASS::~vobsCATALOG_MASS()
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
mcsCOMPL_STAT vobsCATALOG_MASS::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_MASS::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=Jmag&-out=Hmag&-out=Kmag");
    miscDynBufAppendString(&_asking, "&-out=*POS_GAL_LAT&-out=*POS_GAL_LON");
            
    return SUCCESS;
}

/*___oOo___*/
