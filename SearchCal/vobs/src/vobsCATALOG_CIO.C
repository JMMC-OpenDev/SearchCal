/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsCATALOG_CIO.C,v 1.1 2004-07-28 14:18:05 scetre Exp $"
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

static char *rcsId="@(#) $Id: vobsCATALOG_CIO.C,v 1.1 2004-07-28 14:18:05 scetre Exp $"; 
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
 * \return SUCCESS on successful completion. Otherwise FAILURE is returned.
 * 
 * \b Errors codes:\n 
 * The possible errors are:
 *
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteAskingConstant(void)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingConstant()");

    miscDynStrAppendString(&_asking,"&-file=-c&-c.eq=J2000&&x_F(IR)=M");
    miscDynStrAppendString(&_asking,"&lambda=1.25,1.65,2.20");
    miscDynStrAppendString(&_asking,"&-out.max=50");
    miscDynStrAppendString(&_asking,"-c.r=1&-c.u=arcmin");
    
    return SUCCESS;
}



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
mcsCOMPL_STAT vobsCATALOG_CIO::WriteAskingSpecificParameters(void)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingSpecificParameters()");
   
    miscDynStrAppendString(&_asking, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynStrAppendString(&_asking, "&-out=lambda&-out=F(IR)&-out=x_F(IR)");
    miscDynStrAppendString(&_asking, "&-sort=_r");
            
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
 * \li vobsERR_UNKNOWN_BAND
 */
mcsCOMPL_STAT vobsCATALOG_CIO::WriteAskingSpecificParameters(vobsREQUEST request)
{
    logExtDbg("vobsCATALOG_CIO::GetAskingSpecificParameters()");

    miscDynStrAppendString(&_asking, "&x_F(IR)=M");
    miscDynStrAppendString(&_asking, "&F(IR)=");
    
    char *magRange="";
    request.GetConstraint(MAGNITUDE_RANGE_ID,magRange);
    miscDynStrAppendString(&_asking, magRange);
    miscDynStrAppendString(&_asking, "&lambda=");

    char *lambda="";
    request.GetConstraint(STAR_WLEN_ID,lambda);
    miscDynStrAppendString(&_asking, lambda);

    char *band="";
    request.GetConstraint(OBSERVED_BAND_ID,band);
    if (strcmp(band,"K")==0)
    {
        miscDynStrAppendString(&_asking, "2.20");
    }
    else if (strcmp(band,"H")==0)
    {
        miscDynStrAppendString(&_asking, "1.65");        
    }
    else if (strcmp(band,"J")==0)
    {
        miscDynStrAppendString(&_asking, "1.25");        
    }

    miscDynStrAppendString(&_asking, "&-out.max=50&-c.bm=1800/300&-c.u=arcmin");
    miscDynStrAppendString(&_asking, "&-out.add=_RAJ2000,_DEJ2000&-oc=hms");
    miscDynStrAppendString(&_asking, "&-out=lambda&-out=F(IR)&-out=x_F(IR)");
    miscDynStrAppendString(&_asking, "&-sort=_r");
    
    return SUCCESS;
}


/*___oOo___*/
