/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_DENIS.C,v 1.6 2004-11-23 12:47:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_DENIS class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_DENIS.C,v 1.6 2004-11-23 12:47:48 scetre Exp $"; 
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
#include "vobsCATALOG_DENIS.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_DENIS::vobsCATALOG_DENIS()
{
    strcpy(_name,"B/denis");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_DENIS::~vobsCATALOG_DENIS()
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_DENIS::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_DENIS::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*VELOC_ROTAT");
            
    return SUCCESS;
}



/*___oOo___*/
