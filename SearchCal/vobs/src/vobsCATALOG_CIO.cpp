/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CIO.cpp,v 1.2 2005-01-24 10:58:44 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_CIO class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_CIO.cpp,v 1.2 2005-01-24 10:58:44 scetre Exp $"; 
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
#include "vobsCATALOG_CIO.h"
#include "vobsPrivate.h"
#include "vobsErrors.h"

/*
 * Class constructor
 */

/**
 * Build a catalog object.
 */
vobsCATALOG_CIO::vobsCATALOG_CIO()
{
    strcpy(_name,"II/225/catalog");
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
 * Prepare the asking for the CIO catalog.
 *
 * Prepare the asking according to the request (constraints) for a first ask
 * to the CDS, that's mean that the use of this asking will help to have a
 * list of possible star. If the request is for a single research, this method
 * will write the spécific asking.
 *
 * \param request vobsREQUEST which have all the contraints for the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 *
 * \b Errors codes:\n 
 * The possible errors are:
 *  \li vobsERR_QUERY_WRITE_FAILED
 */
mcsCOMPL_STAT vobsCATALOG_CIO::PrepareQuery(vobsREQUEST &request)
{
    logExtDbg("vobsCATALOG::PrepareQuery()");

    if ((WriteQueryURIPart()==mcsFAILURE) ||
        (WriteReferenceStarPosition(request)==mcsFAILURE) ||
        (WriteQuerySpecificPart(request)==mcsFAILURE) )
    {
        errAdd(vobsERR_QUERY_WRITE_FAILED, _query);
        return mcsFAILURE;
    }
    return mcsSUCCESS;
}

/**
 * Build the constant part of the asking for CIO catalog
 *
 * Build the constant part of the asking. For each catalog, a part of the
 * asking is the same.
 *
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQueryConstantPart(void)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingConstant()");

    miscDynBufAppendString(&_query,"&-file=-c&-c.eq=J2000&&x_F(IR)=M");
    miscDynBufAppendString(&_query,"&lambda=1.25,1.65,2.20");
    miscDynBufAppendString(&_query,"&-out.max=50");
    miscDynBufAppendString(&_query,"-c.r=1&-c.u=arcmin");
    
    return mcsSUCCESS;
}



/**
 * Build the specificatic part of the asking.
 *
 * Build the specificatic part of the asking. This is the part of the asking
 * which is write specificaly for each catalog.
 *
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda&-out=F(IR)&-out=x_F(IR)");
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
 * \param request vobsREQUEST which help to restrict the search
 *
 * \return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 * \li vobsERR_UNKNOWN_BAND
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteQuerySpecificPart(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&x_F(IR)=M");
    miscDynBufAppendString(&_query, "&F(IR)=");
    
    mcsSTRING32 minMagRange;
    request.GetConstraint(MIN_MAGNITUDE_RANGE_ID,minMagRange);
    miscDynBufAppendString(&_query, minMagRange);
    miscDynBufAppendString(&_query, "..");
    mcsSTRING32 maxMagRange;
    request.GetConstraint(MAX_MAGNITUDE_RANGE_ID,maxMagRange);
    miscDynBufAppendString(&_query, maxMagRange);
    
    miscDynBufAppendString(&_query, "&lambda=");

    /*mcsSTRING32 lambda;
    request.GetConstraint(STAR_WLEN_ID,lambda);
    miscDynBufAppendString(&_query, lambda);*/

    mcsSTRING32 band;
    request.GetConstraint(OBSERVED_BAND_ID, band);
    request.GetConstraint(OBSERVED_BAND_ID,band);
    if (strcmp(band,"K")==0)
    {
        miscDynBufAppendString(&_query, "2.20");
    }
    else if (strcmp(band,"H")==0)
    {
        miscDynBufAppendString(&_query, "1.65");        
    }
    else if (strcmp(band,"J")==0)
    {
        miscDynBufAppendString(&_query, "1.25");        
    }

    miscDynBufAppendString(&_query, "&-out.max=50&-c.bm=");
    mcsSTRING32 searchBoxRa, searchBoxDec;
    request.GetConstraint(SEARCH_BOX_RA_ID,searchBoxRa);
    request.GetConstraint(SEARCH_BOX_DEC_ID,searchBoxDec);
    miscDynBufAppendString(&_query, searchBoxRa);
    miscDynBufAppendString(&_query, "/");
    miscDynBufAppendString(&_query, searchBoxDec);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=lambda&-out=F(IR)&-out=x_F(IR)");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return mcsSUCCESS;
}


/*___oOo___*/
