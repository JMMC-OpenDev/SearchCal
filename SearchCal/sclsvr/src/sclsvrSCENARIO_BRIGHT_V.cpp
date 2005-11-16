/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSCENARIO_BRIGHT_V.cpp,v 1.2 2005-11-16 14:28:02 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/11/15 15:00:33  scetre
 * Added scenario K V and N
 *
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_V class.
 */

static char *rcsId="@(#) $Id: sclsvrSCENARIO_BRIGHT_V.cpp,v 1.2 2005-11-16 14:28:02 scetre Exp $"; 
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
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_V::sclsvrSCENARIO_BRIGHT_V()
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
    _request.Copy(*request);

     // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();


    //////////////////////////////////////////////////////////////////////////
    _criteriaList.Clear();
    // Add criteria on right ascension
    if (_criteriaList.Add(vobsSTAR_POS_EQ_RA_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // Add criteria on declinaison
    if (_criteriaList.Add(vobsSTAR_POS_EQ_DEC_MAIN, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (AddEntry(vobsCATALOG_ASCC_ID, &_request, NULL, &_starListS,
                          vobsCOPY) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // The primary list is completed with the query on catalogs I/196,
    // MASS, II/225, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
    // I/196
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
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
    // CHARM
    /*if (AddEntry(vobsCATALOG_CHARM_ID, &_request, &_starListS, &_starListS,
      vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
      {
      return mcsFAILURE;
      }*/
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
                          vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // SBSC
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    // DENIS
    /*if (AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starListS, &_starListS, 
      vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
      {
      return mcsFAILURE;
      }*/
    // DENIS_JK
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;

}

/*___oOo___*/
