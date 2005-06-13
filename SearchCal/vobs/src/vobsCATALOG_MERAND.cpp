/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_MERAND.cpp,v 1.1 2005-06-13 10:19:30 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of vobsCATALOG_MERAND class.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MERAND.cpp,v 1.1 2005-06-13 10:19:30 scetre Exp $"; 
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

/*
 * Local Headers 
 */
#include "vobsCATALOG_MERAND.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_MERAND::vobsCATALOG_MERAND() : 
                                    vobsREMOTE_CATALOG("J/A+A/433/1155")
{
}

/**
 * Class destructor
 */
vobsCATALOG_MERAND::~vobsCATALOG_MERAND()
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
 *
 * \return always mcsSUCCESS
 *
 */
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_MERAND::GetAskingSpecificParameters()");
  
    miscDynBufAppendString(&_query, "&-out=Bmag,Vmag,Jmag,Hmag,Ksmag");
    miscDynBufAppendString(&_query, "&-out=UDdiamKs,e_UDdiam");
            
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
