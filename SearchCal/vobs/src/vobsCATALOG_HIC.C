/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_HIC.C,v 1.3 2004-09-30 07:40:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_HIC class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_HIC.C,v 1.3 2004-09-30 07:40:09 scetre Exp $"; 
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
#include "vobsCATALOG_HIC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_HIC::vobsCATALOG_HIC()
{
    strcpy(_name,"I/196/main");
    strcpy(_fileName,"");
    strcpy(_nameForFile,"I_196_main");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_HIC::~vobsCATALOG_HIC()
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
mcsCOMPL_STAT vobsCATALOG_HIC::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_HIC::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=*POS_GAL_LAT");
    miscDynBufAppendString(&_asking, "&-out=*POS_GAL_LON");
    miscDynBufAppendString(&_asking, "&-out=*VELOC_HC");
            
    return SUCCESS;
}


/*___oOo___*/
