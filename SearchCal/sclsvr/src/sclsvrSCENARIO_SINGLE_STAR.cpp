/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_SINGLE_STAR.cpp,v 1.8 2011-03-03 13:12:52 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.7  2010/09/01 07:28:35  mella
 * clean unused variable
 *
 * Revision 1.6  2007/10/31 11:31:56  gzins
 * Removed some useless code
 *
 * Revision 1.5  2007/06/27 13:00:59  scetre
 * Do not removed science star if present in the resulting list.
 * Updated get star command
 *
 * Revision 1.4  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.3  2006/05/11 13:04:57  mella
 * Changed rcsId declaration to perform good gcc4 and gcc3 compilation
 *
 * Revision 1.2  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.1  2005/11/21 13:51:47  scetre
 * Changed bad scenario name
 * Added scenario for single star research -> updated getStar
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_SINGLE_STAR class.
 */

static char *rcsId __attribute__ ((unused)) ="@(#) $Id: sclsvrSCENARIO_SINGLE_STAR.cpp,v 1.8 2011-03-03 13:12:52 lafrasse Exp $";
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
#include "sclsvrSCENARIO_SINGLE_STAR.h"
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_SINGLE_STAR::sclsvrSCENARIO_SINGLE_STAR(sdbENTRY* progress):
    vobsSCENARIO(progress)
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_SINGLE_STAR::~sclsvrSCENARIO_SINGLE_STAR()
{
}

/*
 * Public methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_SINGLE_STAR::Init(vobsREQUEST * request,
                                               vobsSTAR_LIST &starList)
{
    logTrace("sclsvrSCENARIO_SINGLE_STAR::Init()");

    // Clear the scenario
    Clear();
    _request.Copy(*request);

    // Clear the storage list 
    _starListS.Clear();

    // Build criteriaList used
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
    _criteriaListRaDecMagV.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add mgV criteria
    if (_criteriaListRaDecMagV.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Decisionnal scenario
    vobsSCENARIO scenarioCheck(_progress);
    // Initialize it
    if (scenarioCheck.AddEntry(vobsCATALOG_ASCC_ID, &_request, &starList,
                               &starList, vobsUPDATE_ONLY, &_criteriaListRaDec)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Set catalog list
    vobsCATALOG_LIST catalogList;
    scenarioCheck.SetCatalogList(&catalogList);
    // Run the method to execute the scenario which had been
    // loaded into memory
    if (scenarioCheck.Execute(_starListS) == mcsFAILURE)
    {
        errUserAdd(sclsvrERR_NO_CDS_RETURN);
        return mcsFAILURE;
    }
    
    mcsDOUBLE magV=1;
    if (_starListS.GetNextStar(mcsTRUE)->GetPropertyValue(vobsSTAR_PHOT_JHN_V,
                                                          &magV) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    printf("%f\n", magV);
    // If mag K of the star found is more than 5.0 use faint photo catalogs
    if (magV > 5.0)
    {
        ////////////////////////////////////////////////////////////////////////
        // I/280 
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        ////////////////////////////////////////////////////////////////////////
        // B/denis
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        } 
    }
    else
    {
        ////////////////////////////////////////////////////////////////////////
        // SECONDARY REQUEST
        ////////////////////////////////////////////////////////////////////////
        // The primary list is completed with the query on catalogs II/225, 
        // I/196, 2MASS, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
        ////////////////////////////////////////////////////////////////////////
        // LBSI
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        ////////////////////////////////////////////////////////////////////////
        // MERAND
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        ////////////////////////////////////////////////////////////////////////
        // CHARM2
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        ////////////////////////////////////////////////////////////////////////
        // DENIS_JK
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, 
                     &_starListS, vobsUPDATE_ONLY,
                     &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }
        ////////////////////////////////////////////////////////////////////////
        // 2MASS
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }

        ////////////////////////////////////////////////////////////////////////
        // II/7A
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDecMagV) ==
            mcsFAILURE)
        {
            return mcsFAILURE;
        }
        ////////////////////////////////////////////////////////////////////////
        // II/225
        ////////////////////////////////////////////////////////////////////////
        if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                     vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
        {
            return mcsFAILURE;
        }        
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
