/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_SBSC.C,v 1.3 2004-09-30 07:40:09 scetre Exp $"
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

static char *rcsId="@(#) $Id: vobsCATALOG_SBSC.C,v 1.3 2004-09-30 07:40:09 scetre Exp $"; 
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
    strcpy(_fileName,"");
    strcpy(_nameForFile,"B_36B_bsc4s");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_SBSC::~vobsCATALOG_SBSC()
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
mcsCOMPL_STAT vobsCATALOG_SBSC::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_SBSC::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=*VELOC_ROTAT");
            
    return SUCCESS;
}

/*___oOo___*/
