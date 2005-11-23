/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_MASS.cpp,v 1.19 2005-11-23 08:34:31 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.18  2005/11/21 13:47:57  scetre
* arrange properties when the URL is written
*
* Revision 1.17  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.16  2005/11/16 10:47:54  scetre
* Updated documentation
*
* Revision 1.15  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.14  2005/10/19 08:44:08  scetre
* remove false unit 'arsec' in URL
*
* Revision 1.13  2005/10/14 08:44:24  scetre
* Updated p77 according to JMMC-MEM-2600-0004
*
* Revision 1.12  2005/09/08 08:25:15  scetre
* remove Qflag=AAA in secondary request
*
* Revision 1.11  2005/09/08 07:54:55  scetre
* Add -opt=T
*
* Revision 1.10  2005/08/08 11:48:44  scetre
* remuve -c.bm
*
* Revision 1.9  2005/08/03 13:58:56  scetre
* Added method to call 2mass catalog as a primary catalog
* Added method WriteQuerySpecificPart(vobsREQUEST &request)
*
* Revision 1.8  2005/06/13 10:20:32  scetre
* Added Qflag in query
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
 * vobsCATALOG_MASS class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_MASS.cpp,v 1.19 2005-11-23 08:34:31 scetre Exp $"; 
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
 * @return always mcsSUCCESS 
 *
 */
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_MASS::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=*ID_CATALOG");
    miscDynBufAppendString(&_query, "&-out=*CODE_QUALITY");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LAT");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LON");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_V");
    // constraints
    miscDynBufAppendString(&_query, "&opt=,T,U");
            
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
mcsCOMPL_STAT vobsCATALOG_MASS::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_MASS::GetAskingSpecificParameters()");

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
    deltaRa = request.GetDeltaRa();
    deltaDec = request.GetDeltaDec();
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);

    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&opt=,T,U");
    miscDynBufAppendString(&_query, "&-out.max=150");
    miscDynBufAppendString(&_query, "&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LAT");
    miscDynBufAppendString(&_query, "&-out=*POS_GAL_LON");
    miscDynBufAppendString(&_query, "&-out=*CODE_QUALITY");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Kmag");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_PHG_V");
    miscDynBufAppendString(&_query, "&-out=*ID_CATALOG");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}

/*___oOo___*/
