/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_DENIS_JK.cpp,v 1.3 2005-11-16 10:47:55 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.1  2005/06/13 10:19:30  scetre
 * Added Charm2, Merand and new Denis catalogs
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_DENIS_JK class.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_DENIS_JK.cpp,v 1.3 2005-11-16 10:47:55 scetre Exp $"; 
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
#include "vobsCATALOG_DENIS_JK.h"
#include "vobsPrivate.h"

/**
 * Class constructor
 */
vobsCATALOG_DENIS_JK::vobsCATALOG_DENIS_JK() 
    : vobsREMOTE_CATALOG("J/A+A/413/1037")
{
}

/**
 * Class destructor
 */
vobsCATALOG_DENIS_JK::~vobsCATALOG_DENIS_JK()
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
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_DENIS_JK::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_DENIS_JK::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=Jmag,Ksmag");
    miscDynBufAppendString(&_query, "&-out=Var&Var=%3C4");
            
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
