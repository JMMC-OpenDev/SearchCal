/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.10 2005-11-30 10:45:30 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.9  2005/11/30 10:35:21  scetre
 * Updated Filter without name
 * Updated scenario
 *
 * Revision 1.8  2005/11/29 13:49:30  scetre
 * Order the scenario to get coordinates from I/280 or I/284
 *
 * Revision 1.7  2005/11/29 13:04:42  scetre
 * Used vobsSCENARIO instead of obsolete sclsvrSCEANRIO_CHECK
 *
 * Revision 1.6  2005/11/29 10:39:03  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.5  2005/11/29 08:23:19  scetre
 * Added check scenario for scenario faint K
 *
 * Revision 1.4  2005/11/25 08:45:55  scetre
 * Added filter Qflag=AAA
 *
 * Revision 1.3  2005/11/24 15:14:02  scetre
 * Enabled the filters used
 *
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

static char *rcsId="@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.10 2005-11-30 10:45:30 scetre Exp $"; 
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

#include "alx.h"
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
/**
 * Initialize the faint K scenario
 *
 * @param request user request
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned
 */
mcsCOMPL_STAT sclsvrSCENARIO_FAINT_K::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_FAINT_K::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS1.Clear();
    _starListS2.Clear();

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
    // Build Filter used Qflg=AAA
    _filterOnQflag.SetPropertyId(vobsSTAR_CODE_QUALITY);
    if (_filterOnQflag.AddCondition("AAA") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOnQflag.Enable();    

    ///////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST
    ///////////////////////////////////////////////////////////////////////////

    // Get Radius entering by the user
    mcsFLOAT radius;
    if (request->GetSearchArea(radius) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // if radius is not set (i.e equal zero)
    // compute radius from alx
    if (radius == 0)
    {
        mcsSTRING32 raString;
        strcpy(raString, request->GetObjectRa());
        mcsSTRING32 decString;
        strcpy(decString, request->GetObjectDec());
        vobsSTAR star;
        star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, raString, "no origin");
        star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, decString, "no origin");

        mcsFLOAT ra;
        mcsFLOAT dec;
        star.GetRa(ra);
        star.GetDec(dec);
        
        mcsFLOAT magMin;
        mcsFLOAT magMax;
        magMin = request->GetMinMagRange();
        magMax = request->GetMaxMagRange();
        
        if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &radius) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
        //
        //  COMPUTE RADIUS with alx
        //
        printf("radius = %f\n", radius);

        // Decisionnal scenario
        vobsSCENARIO scenarioCheck;
        vobsSTAR_LIST starList;
        // Initialize it
        if (scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL,
                                   &starList, vobsCOPY) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        // Set catalog list
        vobsCATALOG_LIST catalogList;
        scenarioCheck.SetCatalogList(&catalogList);
        // Run the method to execute the scenario which had been
        // loaded into memory
        if (scenarioCheck.Execute(_starListP) == mcsFAILURE)
        {
            errUserAdd(vobsERR_NO_CDS_RETURN);
            return mcsFAILURE;
        }
        // If the return is lower than 25 star, twice the radius and recall
        // 2mass
        if (_starListP.Size() < 25)
        {
            if (_request.SetSearchArea(2*radius) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            ///////////////////////////////////////////////////////////////////
            // II/246
            ///////////////////////////////////////////////////////////////////
            if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                         vobsCOPY) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // else if radius is defined, simply query 2mass
    else
    {
        ///////////////////////////////////////////////////////////////////////
        // II/246
        ///////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                     vobsCOPY) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Filter on opt=U
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, 
                vobsCOPY, NULL, &_filterOptT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    ///////////////////////////////////////////////////////////////////////////
    // Filter on opt=U
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, 
                vobsCOPY, NULL, &_filterOptU) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // query on I/280 with S1
    ///////////////////////////////////////////////////////////////////////////
    // I/280 with 
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // query on I/284 with S2
    ///////////////////////////////////////////////////////////////////////////
    // I/284-UNSO
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_UNSO_ID, &_request, &_starListS2, &_starListS2,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Merge S2 and S1
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1,
                 vobsMERGE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // Update this new list S1 with P, S1 = reference list
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ///////////////////////////////////////////////////////////////////////////
    // B/denis
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    ///////////////////////////////////////////////////////////////////////////
    // charm2
    ///////////////////////////////////////////////////////////////////////////
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS1, &_starListS1,
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
