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
sclsvrSCENARIO_JSDC::sclsvrSCENARIO_JSDC(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
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

/**
 * Return the name of this scenario
 * @return "JSDC"
 */
const char* sclsvrSCENARIO_JSDC::GetScenarioName()
{
    return "JSDC_FAST";
}

/**
 * Initialize the JSDC scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC::Init(vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    logTrace("sclsvrSCENARIO_JSDC::Init()");

    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    ////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST on LOCAL CATALOG
    ////////////////////////////////////////////////////////////////////////

    // Use RA/Dec criteria to filter duplicates within 1 arcsec:
    FAIL(AddEntry(vobsCATALOG_ASCC_LOCAL_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // I/311 to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // DENIS_JK
    FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // 2MASS
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec, NULL, "&opt=%5bTU%5d"));

    // II/7A
    FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMagV));

    // II/225
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
