/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_MASS.cpp,v 1.7 2005-06-01 14:16:55 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.6  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.5  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
* Revision 1.4  2005/02/08 20:30:37  gzins
* Changed _name type; mcsSTRING to string
*
* Revision 1.3  2005/01/26 08:11:28  scetre
* change history
*
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_MASS class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MASS.cpp,v 1.7 2005-06-01 14:16:55 scetre Exp $"; 
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
vobsCATALOG_MASS::vobsCATALOG_MASS() : vobsREMOTE_CATALOG("II/246/out")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_MASS::~vobsCATALOG_MASS()
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
 * \return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_MASS::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=Jmag&-out=Hmag&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LAT&-out=*POS_GAL_LON");
            
    return mcsSUCCESS;
}

/*___oOo___*/
