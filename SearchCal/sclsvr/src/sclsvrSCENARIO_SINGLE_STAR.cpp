/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Definition of sclsvrSCENARIO_SINGLE_STAR class.
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
#include "sclsvrSCENARIO_SINGLE_STAR.h"
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_SINGLE_STAR::sclsvrSCENARIO_SINGLE_STAR(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
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

/**
 * Return the name of this scenario
 * @return "SINGLE_STAR"
 */
const char* sclsvrSCENARIO_SINGLE_STAR::GetScenarioName()
{
    return "SINGLE_STAR";
}

mcsCOMPL_STAT sclsvrSCENARIO_SINGLE_STAR::Init(vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    logTrace("sclsvrSCENARIO_SINGLE_STAR::Init()");

    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // Decisionnal scenario
    vobsSCENARIO scenarioCheck(_progress);
    // define catalog list:
    scenarioCheck.SetCatalogList(GetCatalogList());

    // Initialize it
    FAIL(scenarioCheck.AddEntry(vobsCATALOG_ASCC_ID, &_request, starList, starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // Run the method to execute the scenario which had been
    // loaded into memory
    FAIL_DO(scenarioCheck.Execute(_starList), errUserAdd(sclsvrERR_NO_CDS_RETURN));

    ////////////////////////////////////////////////////////////////////////
    // I/280 
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////
    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, CHARM, II/7A, BSC, SBSC, DENIS
    ////////////////////////////////////////////////////////////////////////

    // I/311 to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // DENIS_JK
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // 2MASS
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // LBSI
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_LBSI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // MERAND
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_MERAND_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // II/7A
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // II/225
    ////////////////////////////////////////////////////////////////////////
    FAIL(AddEntry(vobsCATALOG_CIO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // I/196
    FAIL(AddEntry(vobsCATALOG_HIC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // BSC
    FAIL(AddEntry(vobsCATALOG_BSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // SBSC
    FAIL(AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // B/sb9
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // B/wds/wds
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/297/irc aka AKARI
    FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecAkari));

    return mcsSUCCESS;
}


/*___oOo___*/
