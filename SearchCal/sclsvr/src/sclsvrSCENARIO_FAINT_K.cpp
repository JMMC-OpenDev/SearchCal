/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.3 2005-11-24 15:14:02 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.2  2005/11/24 13:21:38  scetre
 * Clean the presentation of the add entry
 *
 * Revision 1.1  2005/11/23 08:39:03  scetre
 * Added scenario for faint K
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_FAINT_K class.
 */

static char *rcsId="@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.3 2005-11-24 15:14:02 scetre Exp $"; 
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
#include "sclsvrSCENARIO_FAINT_K.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_FAINT_K::sclsvrSCENARIO_FAINT_K()
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_FAINT_K::~sclsvrSCENARIO_FAINT_K()
{
}

/*
 * Public methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_FAINT_K::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_FAINT_K::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    
    //////////////////////////////////////////////////////////////////////////
    // BUILD REQUEST USED
    //////////////////////////////////////////////////////////////////////////
    _request.Copy(*request);

    //////////////////////////////////////////////////////////////////////////
    // BUILD CRITERIA LIST
    //////////////////////////////////////////////////////////////////////////
    _criteriaListRaDec.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    //////////////////////////////////////////////////////////////////////////
    // BUILD FILTER USED
    //////////////////////////////////////////////////////////////////////////
    // Build Filter used opt=T
    _filterOptT.SetPropertyId(vobsSTAR_ID_CATALOG);
    if (_filterOptT.AddCondition("T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptT.Enable();
    // Build Filter used opt=T
    _filterOptU.SetPropertyId(vobsSTAR_ID_CATALOG);
    if (_filterOptU.AddCondition("U") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptU.Enable();
    
    ///////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST
    ///////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////
    // II/246
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                 vobsCOPY) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Filter on opt=U
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, 
                vobsNO_ACTION, NULL, &_filterOptT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    ///////////////////////////////////////////////////////////////////////////
    // Filter on opt=U
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, 
                vobsNO_ACTION, NULL, &_filterOptU) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // query on I/280 with S1
    ///////////////////////////////////////////////////////////////////////////
    // I/280 with 
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListP,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // query on I/284 with L2
    ///////////////////////////////////////////////////////////////////////////
    // I/284-UNSO
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_UNSO_ID, &_request, &_starListS2, &_starListP,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    ///////////////////////////////////////////////////////////////////////////
    // B/denis
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListP, &_starListP,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    ///////////////////////////////////////////////////////////////////////////
    // charm2
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListP, &_starListP,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
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
