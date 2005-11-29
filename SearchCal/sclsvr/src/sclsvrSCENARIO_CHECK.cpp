/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_CHECK.cpp,v 1.1 2005-11-29 08:23:19 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_CHECK class.
 */

static char *rcsId="@(#) $Id: sclsvrSCENARIO_CHECK.cpp,v 1.1 2005-11-29 08:23:19 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

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
#include "sclsvrSCENARIO_CHECK.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_CHECK::sclsvrSCENARIO_CHECK()
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_CHECK::~sclsvrSCENARIO_CHECK()
{
}

/*
 * Public methods
 */
/**
 * Initialize decisionnal scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_CHECK::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_CHECK::Init()");
    
    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
   
    //////////////////////////////////////////////////////////////////////////
    // BUILD REQUEST USED
    //////////////////////////////////////////////////////////////////////////
    _request.Copy(*request);

    ///////////////////////////////////////////////////////////////////////////
    // II/246
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                 vobsCOPY) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
