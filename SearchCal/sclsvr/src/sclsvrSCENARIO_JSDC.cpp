/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_JSDC class.
 */


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
#include "sclsvrSCENARIO_JSDC.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_JSDC::sclsvrSCENARIO_JSDC(sdbENTRY* progress):
    vobsSCENARIO(progress)
{
    // enable saving intermediate results after Search and Merge operations:
    _saveSearchXml    = false;
    _saveSearchList   = true;
    _saveMergedList   = true;
    _filterDuplicates = true;
}

/**
 * Class destructor
 */
sclsvrSCENARIO_JSDC::~sclsvrSCENARIO_JSDC()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC"
 */
const char* sclsvrSCENARIO_JSDC::GetScenarioName()
{
    return "JSDC_FAST";
}

/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC::Init(vobsREQUEST * request)
{
    vobsSTAR_COMP_CRITERIA_LIST _criteriaListRaDecAkari;

    logTrace("sclsvrSCENARIO_JSDC::Init()");

    // Clear the scenario
    Clear();
    _request.Copy(*request);

    // Clear the storage lists
    _starListS.Clear();

    // Build criteriaList used
    //////////////////////////////////////////////////////////////////////////
    _criteriaListRaDec.Clear();
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDec.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build criteria list on ra dec and V
    _criteriaListRaDecMagV.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecMagV.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Add mgV criteria
    if (_criteriaListRaDecMagV.Add(vobsSTAR_PHOT_JHN_V, 0.1) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build criteria list on ra dec and hd
    _criteriaListRaDecHd.Clear();
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_RA_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecHd.Add(vobsSTAR_POS_EQ_DEC_MAIN, sclsvrARCSEC_IN_DEGREES) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Add hd criteria
    if (_criteriaListRaDecHd.Add(vobsSTAR_ID_HD, 0) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //AKARI has a 2.4 HPBW for 9 and 18 mu, so 2 arc sec is necessary and OK
    _criteriaListRaDecAkari.Clear();
    // Add Criteria on coordinates
    if (_criteriaListRaDecAkari.Add(vobsSTAR_POS_EQ_RA_MAIN, 2. * sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    if (_criteriaListRaDecAkari.Add(vobsSTAR_POS_EQ_DEC_MAIN, 2. * sclsvrARCSEC_IN_DEGREES) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    ////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST on LOCAL CATALOG
    ////////////////////////////////////////////////////////////////////////

    // Use RA/Dec criteria to filter duplicates within 1 arcsec:
    if (AddEntry(vobsCATALOG_ASCC_LOCAL_ID, &_request,  NULL, &_starListS, 
            vobsCOPY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // DENIS_JK
    if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS, 
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // 2MASS
    if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec, NULL, "&opt=T") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecMagV) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/225
    if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }        

    // I/196
    if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // BSC
    if (AddEntry(vobsCATALOG_BSC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // SBSC
    if (AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/sb9
    if (AddEntry(vobsCATALOG_SB9_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // B/wsd/wsd
    if (AddEntry(vobsCATALOG_WDS_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // II/297/irc aka AKARI
    if (AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDecAkari) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

/*___oOo___*/
