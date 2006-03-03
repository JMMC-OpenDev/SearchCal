/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_CHARM2.cpp,v 1.5 2006-03-03 15:03:27 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/11/21 13:47:57  scetre
 * arrange properties when the URL is written
 *
 * Revision 1.3  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.2  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/13 10:19:30  scetre
 * Added Charm2, Merand and new Denis catalogs
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_CHARM2 class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_CHARM2.cpp,v 1.5 2006-03-03 15:03:27 scetre Exp $"; 

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

/*
 * Local Headers 
 */
#include "vobsCATALOG_CHARM2.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_CHARM2::vobsCATALOG_CHARM2() :
                                    vobsREMOTE_CATALOG("J/A+A/431/773/charm2")
{
}

/**
 * Class destructor
 */
vobsCATALOG_CHARM2::~vobsCATALOG_CHARM2()
{
    miscDynBufDestroy(&_query);    
}

/*
 * Public methods
 */


/*
 * Protected methods
 */
/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_CHARM2::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_CHARM2::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=LD");
    miscDynBufAppendString(&_query, "&-out=e_LD");
    miscDynBufAppendString(&_query, "&-out=UD");
    miscDynBufAppendString(&_query, "&-out=e_UD");
    miscDynBufAppendString(&_query, "&-out=Method");
    miscDynBufAppendString(&_query, "&-out=Lambda");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    // constraint        
    miscDynBufAppendString(&_query, "&Method=,LBI,LO,SPE");
    
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
