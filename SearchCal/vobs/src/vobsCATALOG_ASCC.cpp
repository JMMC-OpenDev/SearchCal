/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_ASCC.cpp,v 1.25 2011-03-03 13:09:42 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.24  2010/06/28 14:12:11  lafrasse
* Moved CDS return limit to 1000.
*
* Revision 1.23  2006/08/22 15:42:58  gzins
* Added TYC2 & TYC3
*
* Revision 1.22  2006/03/28 12:57:21  scetre
* Removed Spectral Type constraints
*
* Revision 1.21  2006/03/03 15:03:27  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.20  2006/02/20 16:30:49  scetre
* Patched with option -c.geom=b in request
*
* Revision 1.19  2005/12/05 13:07:50  scetre
* Added error on paralax
*
* Revision 1.18  2005/11/23 17:30:20  lafrasse
* Added circular search box geometry support and normalized area size methods
*
* Revision 1.17  2005/11/23 08:34:31  scetre
* Added property for faint K scenario
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
* Revision 1.13  2005/11/15 14:57:56  scetre
* Added new scenario structure
* Added possibility to query merand and borde as primary catalog
*
* Revision 1.12  2005/10/14 08:44:24  scetre
* Updated p77 according to JMMC-MEM-2600-0004
*
* Revision 1.11  2005/06/01 14:16:55  scetre
* Changed logExtDbg to logTrace
*
* Revision 1.10  2005/04/14 14:39:03  scetre
* Updated documentation.
* added test on method return.
*
* Revision 1.9  2005/02/16 13:21:47  scetre
* changed armin in arcsec in query
*
* Revision 1.8  2005/02/14 15:22:44  scetre
* changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
*
* Revision 1.7  2005/02/13 15:29:04  gzins
* Change Min/MaxDeltaMag to Min/MaxMagRange
*
* Revision 1.6  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
* Revision 1.5  2005/02/08 20:30:37  gzins
* Changed _name type; mcsSTRING to string
*
* Revision 1.4  2005/02/07 19:40:58  gzins
* Updated vobsREQUEST API
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
 * Definition vobsCATALOG_ASCC class.
 */


static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_ASCC.cpp,v 1.25 2011-03-03 13:09:42 lafrasse Exp $"; 

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
#include "vobsCATALOG_ASCC.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_ASCC::vobsCATALOG_ASCC() : vobsREMOTE_CATALOG("I/280")
{
}


/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_ASCC::~vobsCATALOG_ASCC()
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
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_ASCC::GetAskingSpecificParameters()");
    
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=e_Plx");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=v1");
    miscDynBufAppendString(&_query, "&-out=v2");
    miscDynBufAppendString(&_query, "&-out=v3");
    miscDynBufAppendString(&_query, "&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP");
    miscDynBufAppendString(&_query, "&-out=HD");
    miscDynBufAppendString(&_query, "&-out=DM");
    miscDynBufAppendString(&_query, "&-out=TYC1");
    miscDynBufAppendString(&_query, "&-out=TYC2");
    miscDynBufAppendString(&_query, "&-out=TYC3");
    // constraints
    
    miscDynBufAppendString(&_query, "&-sort=_r");
    
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
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_ASCC::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&");
    
    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    miscDynBufAppendString(&_query, band);
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange;
    mcsDOUBLE maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    miscDynBufAppendString(&_query, "mag=");
    miscDynBufAppendString(&_query, rangeMag);

    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);

//    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=100&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=1000&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    // properties to retreive
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=e_Plx");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=v1");
    miscDynBufAppendString(&_query, "&-out=v2");
    miscDynBufAppendString(&_query, "&-out=v3");
    miscDynBufAppendString(&_query, "&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP");
    miscDynBufAppendString(&_query, "&-out=HD");
    miscDynBufAppendString(&_query, "&-out=DM");
    miscDynBufAppendString(&_query, "&-out=TYC1");
    // constraints
    miscDynBufAppendString(&_query, "&-sort=_r"); 
    
    return mcsSUCCESS;
}

/*___oOo___*/
