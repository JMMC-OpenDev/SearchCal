/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_LBSI.cpp,v 1.10 2005-11-16 10:47:54 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.9  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.8  2005/08/03 13:59:47  scetre
* Add L, M, N magitude as wanted properties for the LBSI catalog
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
 * vobsCATALOG_LBSI class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_LBSI.cpp,v 1.10 2005-11-16 10:47:54 scetre Exp $"; 
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
#include "vobsCATALOG_LBSI.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_LBSI::vobsCATALOG_LBSI() : 
                                    vobsREMOTE_CATALOG("J/A+A/393/183/catalog")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_LBSI::~vobsCATALOG_LBSI()
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
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_LBSI::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=Bmag,Vmag,Jmag,Hmag,Kmag,");
    miscDynBufAppendString(&_query, "Lmag,Mmag,Nmag");
    miscDynBufAppendString(&_query, "&-out=UDDK,e_UDDK");
            
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * @param request vobsREQUEST which help to restrict the search
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_LBSI::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_LBSI::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&");
    
    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    miscDynBufAppendString(&_query, band);
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsFLOAT minMagRange;
    mcsFLOAT maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);

    // Add search box size
    mcsSTRING32 separation;
    mcsFLOAT deltaRa;
    mcsFLOAT deltaDec;
    deltaRa = request.GetDeltaRa();
    deltaDec = request.GetDeltaDec();
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);
    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=100&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=Bmag,Vmag,Jmag,Hmag,Kmag,");
    miscDynBufAppendString(&_query, "Lmag,Mmag,Nmag");
    miscDynBufAppendString(&_query, "&-out=UDDK,e_UDDK");
    
    return mcsSUCCESS;
}

/*___oOo___*/
