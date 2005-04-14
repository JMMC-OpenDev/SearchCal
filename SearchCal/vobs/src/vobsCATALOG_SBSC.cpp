/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_SBSC.cpp,v 1.6 2005-04-14 14:39:03 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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
 * vobsCATALOG_SBSC class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_SBSC.cpp,v 1.6 2005-04-14 14:39:03 scetre Exp $"; 
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
vobsCATALOG_SBSC::vobsCATALOG_SBSC() : vobsREMOTE_CATALOG("V/36B/bsc4s")
{
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
 * \return always mcsSUCCESS
 *
 */
mcsCOMPL_STAT vobsCATALOG_SBSC::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_SBSC::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*VELOC_ROTAT");
            
    return mcsSUCCESS;
}

/*___oOo___*/
