/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CHARM.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_CHARM class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_CHARM.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"; 
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
#include "vobsCATALOG_CHARM.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_CHARM::vobsCATALOG_CHARM()
{
    strcpy(_name,"J/A+A/386/492/charm");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_CHARM::~vobsCATALOG_CHARM()
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
mcsCOMPL_STAT vobsCATALOG_CHARM::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_CHARM::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=*POS_EQ_RA_MAIN");
    miscDynBufAppendString(&_asking, "&-out=*POS_EQ_DEC_MAIN&-out=LD");
    miscDynBufAppendString(&_asking, "&-out=e_LD&-out=UD&-out=e_UD");
    miscDynBufAppendString(&_asking, "&-out=*OBS_METHOD&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_asking, "&Inst=,LBI,LO,SPE&-out=Lambda");
            
    return SUCCESS;
}

/*___oOo___*/
