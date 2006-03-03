/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_DENIS.cpp,v 1.11 2006-03-03 15:03:27 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.10  2005/11/23 08:34:31  scetre
* Added property for faint K scenario
*
* Revision 1.9  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.8  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.7  2005/06/01 14:16:55  scetre
* Changed logExtDbg to logTrace
*
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
 * @file
 * vobsCATALOG_DENIS class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_DENIS.cpp,v 1.11 2006-03-03 15:03:27 scetre Exp $"; 

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
vobsCATALOG_DENIS::vobsCATALOG_DENIS() : vobsREMOTE_CATALOG("B/denis")
{
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
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_DENIS::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_DENIS::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_RA_OTHER");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_DEC_OTHER");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "&-out=*PHOT_COUS_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_B");
    miscDynBufAppendString(&_query, "&-out=Iflg");
    miscDynBufAppendString(&_query, "&-out=Jflg");
    miscDynBufAppendString(&_query, "&-out=Kflg");
    miscDynBufAppendString(&_query, "&-out=DENIS");
            
    return mcsSUCCESS;
}



/*___oOo___*/
