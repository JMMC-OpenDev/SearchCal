/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_V.cpp,v 1.13 2009-04-20 14:41:41 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.12  2009/04/20 14:37:42  lafrasse
 * Added spectral binary detection (SBC9 catalog).
 *
 * Revision 1.11  2007/10/31 11:31:25  gzins
 * Code indentation
 *
 * Revision 1.10  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.9  2006/03/28 12:58:13  scetre
 * Added parameters constraints on I/280 query
 *
 * Revision 1.8  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.7  2005/12/14 09:02:35  scetre
 * Changed constructor in order to handle the filter of the scenario classes
 *
 * Revision 1.6  2005/12/07 14:51:42  scetre
 * Used new generic filter
 *
 * Revision 1.5  2005/11/29 10:39:03  gzins
 * Changed vobsBASE_FILTER to vobsFILTER
 *
 * Revision 1.4  2005/11/24 13:21:38  scetre
 * Clean the presentation of the add entry
 *
 * Revision 1.3  2005/11/23 08:38:14  scetre
 * ReAdd criteria on HD number for cross matching for I/196 and BSC/SBSC
 *
 * Revision 1.2  2005/11/16 14:28:02  scetre
 * Used criteria list as member of the class. All necessary criteria list are now built at the beginning of the Init() method
 *
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_V class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSCENARIO_BRIGHT_V.cpp,v 1.13 2009-04-20 14:41:41 lafrasse Exp $"; 

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
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_V::sclsvrSCENARIO_BRIGHT_V(sdbENTRY* progress):
    vobsSCENARIO(progress),
    _filterOptT("Opt = T filter", vobsSTAR_ID_CATALOG)
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_V::~sclsvrSCENARIO_BRIGHT_V()
{
}

/*
 * Public methods
 */


/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_V::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_BRIGHT_V::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListS.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    // Build criteria on ra dec
    _criteriaList.Clear();
    // Add criteria on right ascension
    if (_criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (_criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Build criteria on ra dec and hd
    _criteriaListHd.Clear();
    // Add criteria on right ascension
    if (_criteriaListHd.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (_criteriaListHd.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.00278) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Add crietria on HD
    if (_criteriaListHd.Add(vobsSTAR_ID_HD, 0) == mcsFAILURE)
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

    // REQUEST
    
    // I/280
    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, NULL, &_starListS,
                          vobsCOPY, NULL, NULL, "&SpType=%5bOBAFGKM%5d*") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // The primary list is completed with the query on catalogs I/196,
    // MASS, II/225, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
    // I/196
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // filter opt=T
    if (AddEntry(vobsNO_CATALOG_ID, &_request, &_starListS, &_starListS,
                 vobsCOPY, NULL, &_filterOptT) 
        == mcsFAILURE)
    {
        return mcsFAILURE;
    } 
    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // LBSI
    if (AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // MERAND
    if (AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // CHARM2
    if (AddEntry(vobsCATALOG_CHARM2_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // BSC
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // SBSC
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // DENIS_JK
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    return mcsSUCCESS;

}

/*___oOo___*/
