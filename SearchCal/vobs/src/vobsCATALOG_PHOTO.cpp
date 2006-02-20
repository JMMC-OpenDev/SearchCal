/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_PHOTO.cpp,v 1.18 2006-02-20 16:30:49 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.17  2005/11/23 17:30:21  lafrasse
* Added circular search box geometry support and normalized area size methods
*
* Revision 1.16  2005/11/21 13:47:57  scetre
* arrange properties when the URL is written
*
* Revision 1.15  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.14  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.13  2005/06/01 14:16:55  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.12  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.11  2005/02/16 13:22:00  scetre
* changed armin in arcsec in query
*
* Revision 1.10  2005/02/14 15:22:44  scetre
* changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
*
* Revision 1.9  2005/02/13 15:29:04  gzins
* Change Min/MaxDeltaMag to Min/MaxMagRange
*
* Revision 1.8  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
* Revision 1.7  2005/02/08 20:30:37  gzins
* Changed _name type; mcsSTRING to string
*
* Revision 1.6  2005/02/08 14:25:05  scetre
* Changed bug in writing magnitude range in query
*
* Revision 1.5  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
*
* Revision 1.4  2005/01/27 13:45:49  scetre
* remove bug in http query
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
 * vobsCATALOG_PHOTO class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_PHOTO.cpp,v 1.18 2006-02-20 16:30:49 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std; 
/**
 * Export standard iostream objects (cin, cout,...).
 */

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
#include "vobsCATALOG_PHOTO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_PHOTO::vobsCATALOG_PHOTO() : vobsREMOTE_CATALOG("II/7A/catalog")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_PHOTO::~vobsCATALOG_PHOTO()
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
 * @return always mcsSUCCESS
 *
 */
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_H");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_U");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_M");
    miscDynBufAppendString(&_query, "&-out=*PHOT_IR_N:10.4");
            
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
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");

    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsFLOAT minMagRange;
    mcsFLOAT maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    // Add search box size
    mcsSTRING32 separation;
    mcsFLOAT deltaRa;
    mcsFLOAT deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);
    
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-out.max=50");
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcsec");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_U");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_M");
    miscDynBufAppendString(&_query, "&-out=*PHOT_IR_N:10.4");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}

/*___oOo___*/
