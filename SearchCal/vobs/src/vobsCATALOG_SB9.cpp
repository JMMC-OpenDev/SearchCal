/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_SB9.cpp,v 1.1 2009-02-09 16:23:48 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_SB9 class.
 */

static char *rcsId __attribute__ ((unused)) = "@(#) $Id: vobsCATALOG_SB9.cpp,v 1.1 2009-02-09 16:23:48 lafrasse Exp $"; 

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
#include "vobsCATALOG_SB9.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_SB9::vobsCATALOG_SB9() : vobsREMOTE_CATALOG(vobsCATALOG_SB9_ID)
{
}

/**
 * Class destructor
 */
vobsCATALOG_SB9::~vobsCATALOG_SB9()
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
 * Build the specific part of the CDS query.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 *
 */
mcsCOMPL_STAT vobsCATALOG_SB9::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_SB9::WriteQuerySpecificPart()");
/*    
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
*/
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
