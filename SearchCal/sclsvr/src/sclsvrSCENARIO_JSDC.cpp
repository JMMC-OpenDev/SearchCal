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


/*
 * Protected methods
 */


/*
 * Private methods
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC::Init(vobsREQUEST * request)
{
    logTrace("sclsvrSCENARIO_JSDC::Init()");

    // Clear the scenario
    Clear();
    // Clear the list input and list output which will be used
    _starListP.Clear();
    _starListS.Clear();
    _request.Copy(*request);
    // Build criteria list on ra dec and hd
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


    if (AddEntry(vobsCATALOG_ASCC_LOCAL_ID, &_request,  NULL, &_starListS, vobsCOPY)
        == mcsFAILURE)
    {
        return mcsFAILURE;
    }

     // SECONDARY REQUEST
    // drop denis_jk (too few) and 2MASS (not useful) 
//     // DENIS_JK
//     if (AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starListS, &_starListS,
//                           vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
//     {
//         return mcsFAILURE;
//     }

//     // 2MASS
//     if (AddEntry(vobsCATALOG_MASS_ID, &_request, &_starListS, &_starListS,
//                           vobsUPDATE_ONLY, &_criteriaListRaDec, NULL,
//                           "&opt=T") == mcsFAILURE)
//     {
//         return mcsFAILURE;
//     }

    // II/7A
    if (AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starListS, &_starListS,
                          vobsUPDATE_ONLY, &_criteriaListRaDecMagK) ==
        mcsFAILURE)
    {
        return mcsFAILURE;
    }

    //drop CIO - too long and too few infos
//     // II/225
//     if (AddEntry(vobsCATALOG_CIO_ID, &_request, &_starListS, &_starListS,
//                           vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
//     {
//         return mcsFAILURE;
//     }        
    //drop HIC ?
//     // I/196
//     if (AddEntry(vobsCATALOG_HIC_ID, &_request, &_starListS, &_starListS,
//                           vobsUPDATE_ONLY, &_criteriaListRaDecHd) == mcsFAILURE)
//     {
//         return mcsFAILURE;
//     }

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
    if (AddEntry(vobsCATALOG_AKARI_ID, NULL, &_starListS, &_starListS,
                 vobsUPDATE_ONLY, &_criteriaListRaDec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
   
    return mcsSUCCESS;
}

/*___oOo___*/
