/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_PHOTO.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    28-Jul-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * vobsCATALOG_PHOTO class definition.
 */

static char *rcsId="@(#) $Id: vobsCATALOG_PHOTO.C,v 1.2 2004-08-03 13:44:10 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std; /**< Export standard iostream objects (cin, cout,...) */


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
vobsCATALOG_PHOTO::vobsCATALOG_PHOTO()
{
    strcpy(_name,"II/7A/catalog");
}

/*
 * Class destructor
 */

/**
 * Delete a catalog object. 
 */
vobsCATALOG_PHOTO::~vobsCATALOG_PHOTO()
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_R&-out=*PHOT_JHN_I");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_asking, "-out=*PHOT_JHN_H&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
            
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteAskingSpecificParameters(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_asking, "&");

    mcsSTRING32 band;
    request.GetConstraint(OBSERVED_BAND_ID,band);
    miscDynBufAppendString(&_asking, band);
    miscDynBufAppendString(&_asking, "=");

    mcsSTRING32 magRange;
    request.GetConstraint(MAGNITUDE_RANGE_ID,magRange);
    miscDynBufAppendString(&_asking, magRange);
    miscDynBufAppendString(&_asking, "&-out.max=50&-c.bm=1800/300&-c.u=arcmin");
    miscDynBufAppendString(&_asking, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_R");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_H&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_asking, "&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
    miscDynBufAppendString(&_asking, "&-sort=_r");
    
    return SUCCESS;
}

/*___oOo___*/
