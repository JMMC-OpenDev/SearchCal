/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.23 2009-04-20 14:41:41 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.22  2009/04/20 14:37:42  lafrasse
 * Added spectral binary detection (SBC9 catalog).
 *
 * Revision 1.21  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.20  2006/08/25 06:07:22  gzins
 * Fixed bug related to error handling; vobsERR_xxx errors was added in error stack.
 *
 * Revision 1.19  2006/03/28 12:42:12  gzins
 * Added opt and Qflg option when querying 2mass
 *
 * Revision 1.18  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.17  2006/01/18 08:49:40  scetre
 * Removed unused filter on Qflg and opt because they are now managed by the catalog option
 *
 * Revision 1.16  2005/12/22 10:12:16  scetre
 * Added log
 *
 * Revision 1.15  2005/12/14 09:02:35  scetre
 * Changed constructor in order to handle the filter of the scenario classes
 *
 * Revision 1.14  2005/12/12 14:08:17  scetre
 * Added radius computing
 *
 * Revision 1.13  2005/12/07 15:11:03  scetre
 * Removed unused printf
 *
 * Revision 1.12  2005/12/07 14:51:42  scetre
 * Used new generic filter
 *
 * Revision 1.11  2005/12/06 08:37:54  scetre
 * Prepared computing for faint calibrator
 * Added Qflag filter on faint scenario
 *
 * Revision 1.10  2005/11/30 10:45:30  scetre
 * Removed unused sclsvrSCENARIO_CHECK
 *
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

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_FAINT_K.cpp,v 1.23 2009-04-20 14:41:41 lafrasse Exp $"; 

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
#include "sclsvrErrors.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_FAINT_K::sclsvrSCENARIO_FAINT_K(sdbENTRY* progress):
    vobsSCENARIO(progress),
    _filterOptT("Opt = T filter", vobsSTAR_ID_CATALOG),
    _filterOptU("Opt = U filter", vobsSTAR_ID_CATALOG)
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

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
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
    
    // BUILD FILTER USED
    // Build Filter used opt=T
    if (_filterOptT.AddCondition(vobsEQUAL, "T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptT.Enable();
    // Build Filter used opt=T
    if (_filterOptU.AddCondition(vobsEQUAL, "U") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    _filterOptU.Enable();

    // PRIMARY REQUEST

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
        
        // compute radius with alx
        if (alxGetResearchAreaSize(ra, dec, magMin, magMax, &radius) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
        logInfo("Sky research radius = %.2f(arcmin)", radius);
        
        if (_request.SetSearchArea(radius) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        // Decisionnal scenario
        vobsSCENARIO scenarioCheck(_progress);
        vobsSTAR_LIST starList;
        // Initialize it
        if (scenarioCheck.AddEntry(vobsCATALOG_MASS_ID, &_request, NULL,
                                   &starList, vobsCOPY, NULL, NULL,
                                   "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
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
            errUserAdd(sclsvrERR_NO_CDS_RETURN);
            return mcsFAILURE;
        }
        // If the return is lower than 25 star, twice the radius and recall
        // 2mass
        if (_starListP.Size() < 25)
        {
            if (_request.SetSearchArea(sqrt(2.0)*radius) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logInfo("New Sky research radius = %.2f(arcmin)", sqrt(2.0)*radius);

            // II/246
            if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                         vobsCOPY, NULL, NULL, 
                         "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }
    // else if radius is defined, simply query 2mass
    else
    {
        // II/246
        if (AddEntry(vobsCATALOG_MASS_ID, &_request, NULL, &_starListP,
                     vobsCOPY, NULL, NULL, 
                     "&opt=%5bTU%5d&Qflg=AAA") == mcsFAILURE)
        {
            return mcsFAILURE;
        }
    }

    // Filter on opt=T
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1, 
                vobsCOPY, NULL, &_filterOptT) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    // Filter on opt=U
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS2, 
                vobsCOPY, NULL, &_filterOptU) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // query on I/280 with S1
    // I/280 with 
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS1, &_starListS1,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // query on I/284 with S2
    // I/284-UNSO
    if (AddEntry(vobsCATALOG_UNSO_ID, &_request, &_starListS2, &_starListS2,
                          vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Merge S2 and S1
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS2, &_starListS1,
                 vobsMERGE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // Update this new list S1 with P, S1 = reference list
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListP, &_starListS1,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/denis
    if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS1, &_starListS1,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // charm2
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS1, &_starListS1,
                          vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS1, &_starListS1,
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
