/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_ASCC.cpp,v 1.10 2005-04-14 14:39:03 scetre Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
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
 * \file
 * Definition vobsCATALOG_ASCC class.
 */


static char *rcsId="@(#) $Id: vobsCATALOG_ASCC.cpp,v 1.10 2005-04-14 14:39:03 scetre Exp $"; 
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
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");
    
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=v1&-out=d5&-out=HIP&-out=HD");
    miscDynBufAppendString(&_query, "&-out=DM&-sort=_r");
    
    return mcsSUCCESS;
}

/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog. The constraints of the request
 * which help to build an asking in order to restrict the research.
 *
 * \param request vobsREQUEST which help to restrict the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is 
 * returned.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");

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
    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=50&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=v1&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP&-out=HD&-out=DM&-sort=_r"); 
    
    return mcsSUCCESS;
}

/*___oOo___*/
