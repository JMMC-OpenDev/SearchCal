/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_ASCC.C,v 1.10 2004-11-23 12:47:48 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * vobsCATALOG_ASCC class definition.
 */


static char *rcsId="@(#) $Id: vobsCATALOG_ASCC.C,v 1.10 2004-11-23 12:47:48 scetre Exp $"; 
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
vobsCATALOG_ASCC::vobsCATALOG_ASCC()
{
    strcpy(_name,"I/280");
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
 *
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");
    
    miscDynBufAppendString(&_query,"&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query,"&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query,"&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query,"&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_query,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query,"&-out=v1&-out=d5&-out=HIP&-out=HD");
    miscDynBufAppendString(&_query,"&-out=DM&-sort=_r");
    
    return SUCCESS;
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteQuerySpecificPart(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&");
    mcsSTRING32 band;
    request.GetConstraint(OBSERVED_BAND_ID,band);
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "mag=");
    mcsSTRING32 minMagRange;
    request.GetConstraint(MIN_MAGNITUDE_RANGE_ID,minMagRange);
    miscDynBufAppendString(&_query, minMagRange);
    miscDynBufAppendString(&_query, "..");
    mcsSTRING32 maxMagRange;
    request.GetConstraint(MAX_MAGNITUDE_RANGE_ID,maxMagRange);
    miscDynBufAppendString(&_query, maxMagRange);
    miscDynBufAppendString(&_query, "&-c.eq=J2000&-out.max=100&-c.bm=");
    mcsSTRING32 searchBoxRa, searchBoxDec;
    request.GetConstraint(SEARCH_BOX_RA_ID,searchBoxRa);
    request.GetConstraint(SEARCH_BOX_DEC_ID,searchBoxDec);
    miscDynBufAppendString(&_query, searchBoxRa);
    miscDynBufAppendString(&_query, "/");
    miscDynBufAppendString(&_query, searchBoxDec);
    miscDynBufAppendString(&_query, "&-c.u=arcsec");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_query, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_query, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_query, "&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=v1&-out=d5");
    miscDynBufAppendString(&_query, "&-out=HIP&-out=HD&-out=DM&-sort=_r"); 
    
    return SUCCESS;
}

/*___oOo___*/
