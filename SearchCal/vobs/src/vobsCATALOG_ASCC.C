/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_ASCC.C,v 1.5 2004-09-30 07:40:09 scetre Exp $"
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


static char *rcsId="@(#) $Id: vobsCATALOG_ASCC.C,v 1.5 2004-09-30 07:40:09 scetre Exp $"; 
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
    strcpy(_fileName,"");
    strcpy(_nameForFile,"I_280");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_ASCC::~vobsCATALOG_ASCC()
{
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
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");
    
    miscDynBufAppendString(&_asking,"&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_asking,"&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_asking,"&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_asking,"&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_asking,"&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_asking,"&-out=v1&-out=d5&-out=HIP&-out=HD");
    miscDynBufAppendString(&_asking,"&-out=DM&-sort=_r");
    
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
mcsCOMPL_STAT vobsCATALOG_ASCC::WriteAskingSpecificParameters(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_ASCC::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_asking, "&");
    mcsSTRING32 band;
    request.GetConstraint(OBSERVED_BAND_ID,band);
    miscDynBufAppendString(&_asking, band);
    miscDynBufAppendString(&_asking, "mag=");
    mcsSTRING32 magRange;
    request.GetConstraint(MAGNITUDE_RANGE_ID,magRange);
    miscDynBufAppendString(&_asking, magRange);    
    miscDynBufAppendString(&_asking, "&-c.eq=J2000&-out.max=50&-c.bm=1800/300");
    miscDynBufAppendString(&_asking, "&-c.u=arcmin");
    miscDynBufAppendString(&_asking, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_asking, "&-out=*POS_EQ_PMDEC&-out=*POS_EQ_PMRA");
    miscDynBufAppendString(&_asking, "&-out=*POS_PARLX_TRIG");
    miscDynBufAppendString(&_asking, "&-out=*SPECT_TYPE_MK");
    miscDynBufAppendString(&_asking, "&SpType=%5bOBAFGKM%5d*");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_asking, "&-out=v1&-out=d5");
    miscDynBufAppendString(&_asking, "&-out=HIP&-out=HD&-out=DM&-sort=_r"); 
    
    return SUCCESS;
}

/*___oOo___*/
