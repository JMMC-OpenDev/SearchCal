/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_PHOTO.C,v 1.9 2004-11-23 12:47:48 scetre Exp $"
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

static char *rcsId="@(#) $Id: vobsCATALOG_PHOTO.C,v 1.9 2004-11-23 12:47:48 scetre Exp $"; 
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(void)
{
    logExtDbg("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");
   
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_R&-out=*PHOT_JHN_I");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "-out=*PHOT_JHN_H&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
            
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
mcsCOMPL_STAT vobsCATALOG_PHOTO::WriteQuerySpecificPart(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_PHOTO::GetAskingSpecificParameters()");

    miscDynBufAppendString(&_query, "&");

    mcsSTRING32 band;
    request.GetConstraint(OBSERVED_BAND_ID,band);
    miscDynBufAppendString(&_query, band);
    miscDynBufAppendString(&_query, "=");

    mcsSTRING32 minMagRange;
    request.GetConstraint(MIN_MAGNITUDE_RANGE_ID,minMagRange);
    miscDynBufAppendString(&_query, minMagRange);
    miscDynBufAppendString(&_query, "..");
    mcsSTRING32 maxMagRange;
    request.GetConstraint(MAX_MAGNITUDE_RANGE_ID,maxMagRange);
    miscDynBufAppendString(&_query, maxMagRange);
    
    miscDynBufAppendString(&_query, "&-out.max=100&-c.bm=");
    mcsSTRING32 searchBoxRa, searchBoxDec;
    request.GetConstraint(SEARCH_BOX_RA_ID,searchBoxRa);
    request.GetConstraint(SEARCH_BOX_DEC_ID,searchBoxDec);
    miscDynBufAppendString(&_query, searchBoxRa);
    miscDynBufAppendString(&_query, "/");
    miscDynBufAppendString(&_query, searchBoxDec);
    miscDynBufAppendString(&_query, "&-c.u=arcmin");
    miscDynBufAppendString(&_query, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_B&-out=*PHOT_JHN_V");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_R");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_I&-out=*PHOT_JHN_J");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_H&-out=*PHOT_JHN_K");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_U&-out=*PHOT_JHN_L");
    miscDynBufAppendString(&_query, "&-out=*PHOT_JHN_M&-out=*PHOT_IR_N:10.4");
    miscDynBufAppendString(&_query, "&-sort=_r");
    
    return SUCCESS;
}

/*___oOo___*/
