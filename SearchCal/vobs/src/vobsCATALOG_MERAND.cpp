/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: vobsCATALOG_MERAND.cpp,v 1.10 2011-03-03 13:09:42 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2010/06/28 14:12:11  lafrasse
 * Moved CDS return limit to 1000.
 *
 * Revision 1.8  2006/03/03 15:03:27  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.7  2006/02/20 16:30:49  scetre
 * Patched with option -c.geom=b in request
 *
 * Revision 1.6  2005/11/23 17:30:21  lafrasse
 * Added circular search box geometry support and normalized area size methods
 *
 * Revision 1.5  2005/11/21 13:47:57  scetre
 * arrange properties when the URL is written
 *
 * Revision 1.4  2005/11/16 10:47:55  scetre
 * Updated documentation
 *
 * Revision 1.3  2005/11/16 10:47:54  scetre
 * Updated documentation
 *
 * Revision 1.2  2005/11/15 14:57:56  scetre
 * Added new scenario structure
 * Added possibility to query merand and borde as primary catalog
 *
 * Revision 1.1  2005/06/13 10:19:30  scetre
 * Added Charm2, Merand and new Denis catalogs
 *
 ******************************************************************************/

/**
 * @file
 *  Definition of vobsCATALOG_MERAND class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_MERAND.cpp,v 1.10 2011-03-03 13:09:42 lafrasse Exp $"; 

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
 * @return always mcsSUCCESS
 *
 */
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_MERAND::GetAskingSpecificParameters()");
  
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");
            
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
mcsCOMPL_STAT vobsCATALOG_MERAND::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_MERAND::GetAskingSpecificParameters()");

    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsDOUBLE minMagRange;
    mcsDOUBLE maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    // Add search box size
    mcsSTRING32 separation;
    mcsDOUBLE deltaRa;
    mcsDOUBLE deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);
    
    miscDynBufAppendString(&_query, "&");
    miscDynBufAppendString(&_query, band);
    if (strcmp(band, "K") == 0)
    {
        miscDynBufAppendString(&_query, "smag=");
    }
    else
    {
        miscDynBufAppendString(&_query, "mag=");        
    }
    miscDynBufAppendString(&_query, rangeMag);
    miscDynBufAppendString(&_query, "&-c.eq=J2000");
//    miscDynBufAppendString(&_query, "&-out.max=100");
    miscDynBufAppendString(&_query, "&-out.max=1000");
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=Bmag");
    miscDynBufAppendString(&_query, "&-out=Vmag");
    miscDynBufAppendString(&_query, "&-out=Jmag");
    miscDynBufAppendString(&_query, "&-out=Hmag");
    miscDynBufAppendString(&_query, "&-out=Ksmag");
    miscDynBufAppendString(&_query, "&-out=UDdiamKs");
    miscDynBufAppendString(&_query, "&-out=e_UDdiam");
    
    return mcsSUCCESS;
}


/*
 * Private methods
 */


/*___oOo___*/
