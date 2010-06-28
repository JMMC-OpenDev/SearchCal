/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CIO.cpp,v 1.20 2010-06-28 14:12:11 lafrasse Exp $"
*
* History
* -------
* $Log: not supported by cvs2svn $
* Revision 1.19  2006/03/03 15:03:27  scetre
* Changed rcsId to rcsId __attribute__ ((unused))
*
* Revision 1.18  2006/02/20 16:30:49  scetre
* Patched with option -c.geom=b in request
*
* Revision 1.17  2005/11/23 17:30:20  lafrasse
* Added circular search box geometry support and normalized area size methods
*
* Revision 1.16  2005/11/23 08:33:32  scetre
* Removed obsolete PrepareQuery() method
*
* Revision 1.15  2005/11/21 13:47:57  scetre
* arrange properties when the URL is written
*
* Revision 1.14  2005/11/16 10:47:55  scetre
* Updated documentation
*
* Revision 1.13  2005/11/16 10:47:54  scetre
* Updated documentation
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
* Revision 1.9  2005/02/14 15:22:44  scetre
* changed minRangeMag to minMagRange and maxRangeMag to maxMagRange
*
* Revision 1.8  2005/02/13 15:29:04  gzins
* Change Min/MaxDeltaMag to Min/MaxMagRange
*
* Revision 1.7  2005/02/11 14:22:10  gluck
* - Updated to fit with vobsLOCAL_CATALOG and vobsREMOTE_CATALOG classes introduction
* - Changed catalog naming: suppressed SetName() method in general catalog and put it in initialisation list of specialised catalog
* - Updated some comments
*
* Revision 1.6  2005/02/08 20:30:37  gzins
* Changed _name type; mcsSTRING to string
*
* Revision 1.5  2005/02/08 08:36:13  scetre
* Added query reset before preparing a new query
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
 * vobsCATALOG_CIO class definition.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: vobsCATALOG_CIO.cpp,v 1.20 2010-06-28 14:12:11 lafrasse Exp $"; 

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
#include "vobsCATALOG_CIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_CIO::vobsCATALOG_CIO() : vobsREMOTE_CATALOG("II/225/catalog")
{
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_CIO::~vobsCATALOG_CIO()
{
    miscDynBufDestroy(&_query);
}

/*
 * Protected methods
 */

/**
 * Build the constant part of the asking for CIO catalog
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * @return always mcsSUCCESS.
 *
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQueryConstantPart(void)
{
    logTrace("vobsCATALOG_CIO::GetAskingConstant()");

    miscDynBufAppendString(&_query,"&-file=-c");
    miscDynBufAppendString(&_query, "&-c.eq=J2000");
//    miscDynBufAppendString(&_query,"&-out.max=50");
    miscDynBufAppendString(&_query,"&-out.max=1000");
    miscDynBufAppendString(&_query,"-c.r=1");
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");

    return mcsSUCCESS;
}



/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 * @return always mcsSUCCESS.
 * 
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(void)
{
    logTrace("vobsCATALOG_CIO::GetAskingSpecificParameters()");
   
    // properties to retreive
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    miscDynBufAppendString(&_query, "&-out=x_F(IR)");
    //constraints
    miscDynBufAppendString(&_query, "&&x_F(IR)=M");
    miscDynBufAppendString(&_query,"&lambda=1.25,1.65,2.20,3.5,5.0,10.0");
    
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
 * @return always mcsSUCCESS
 * 
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(vobsREQUEST &request)
{
    logTrace("vobsCATALOG_CIO::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&F(IR)=");
    
    // Add the magnitude range constraint
    mcsSTRING32 rangeMag;
    mcsFLOAT minMagRange;
    mcsFLOAT maxMagRange;
    minMagRange = request.GetMinMagRange();
    maxMagRange = request.GetMaxMagRange();
    sprintf(rangeMag, "%.2f..%.2f", minMagRange, maxMagRange);
    miscDynBufAppendString(&_query, rangeMag);
    
    miscDynBufAppendString(&_query, "&lambda=");


    // Add band constraint
    const char *band;
    band = request.GetSearchBand();
    miscDynBufAppendString(&_query, "&lambda=");
    if (band[0] =='K')
    {
        miscDynBufAppendString(&_query, "2.20");
    }
    else if (band[0] == 'H')
    {
        miscDynBufAppendString(&_query, "1.65");        
    }
    else if (band[0] == 'J')
    {
        miscDynBufAppendString(&_query, "1.25");        
    }

    // Add search box size
    mcsSTRING32 separation;
    mcsFLOAT deltaRa;
    mcsFLOAT deltaDec;
    if (request.GetSearchArea(deltaRa, deltaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    sprintf(separation, "%.0f/%.0f", deltaRa, deltaDec);
//    miscDynBufAppendString(&_query, "&-out.max=50");
    miscDynBufAppendString(&_query, "&-out.max=1000");
    miscDynBufAppendString(&_query, "&-c.geom=b&-c.bm=");
    miscDynBufAppendString(&_query, separation);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000");
    miscDynBufAppendString(&_query, "&-out.add=_DEJ2000");
    miscDynBufAppendString(&_query, "&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda");
    miscDynBufAppendString(&_query, "&-out=F(IR)");
    miscDynBufAppendString(&_query, "&-out=x_F(IR)");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}


/*___oOo___*/
