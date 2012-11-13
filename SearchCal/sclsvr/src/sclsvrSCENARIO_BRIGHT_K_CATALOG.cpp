/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_BRIGHT_K_CATALOG class.
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
#include "sclsvrSCENARIO_BRIGHT_K_CATALOG.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_BRIGHT_K_CATALOG::sclsvrSCENARIO_BRIGHT_K_CATALOG(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main"),
_originFilter("K origin = 2mass filter"),
_magnitudeFilter("K mag filter"),
_filterList("filter List")
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_BRIGHT_K_CATALOG::~sclsvrSCENARIO_BRIGHT_K_CATALOG()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "BRIGHT_K_CATALOG"
 */
const char* sclsvrSCENARIO_BRIGHT_K_CATALOG::GetScenarioName()
{
    return "BRIGHT_K_CATALOG";
}

/**
 * Initialize the BRIGHT K (former JSDC) scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT sclsvrSCENARIO_BRIGHT_K_CATALOG::Init(vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    logTrace("sclsvrSCENARIO_BRIGHT_K_CATALOG::Init()");

    // Clear the storage lists
    _starList.Clear();

    // BUILD REQUEST USED
    // Build General request
    _request.Copy(*request);

    // Build the request for I/280
    _requestI280.Copy(_request);
    _requestI280.SetSearchBand("V");

    const mcsDOUBLE kMaxi = _request.GetMaxMagRange();
    const mcsDOUBLE kMini = _request.GetMinMagRange();

    _requestI280.SetMinMagRange(0.0);
    _requestI280.SetMaxMagRange(kMaxi + 4.0); // GD: Nov 2012: K + 4 to get more giant stars

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    // BUILD FILTER USED
    // Build origin = 2MASS for Kmag filter
    _originFilter.SetOriginName(vobsCATALOG_MASS_ID, vobsSTAR_PHOT_JHN_K);
    _originFilter.Enable();

    // Build filter on magnitude
    // Get research band
    const char* band = _request.GetSearchBand();

    _magnitudeFilter.SetMagnitudeValue(band, 0.5 * (kMaxi + kMini), 0.5 * (kMaxi - kMini));
    _magnitudeFilter.Enable();

    // Build filter list
    _filterList.Add(&_originFilter, "Origin Filter");
    _filterList.Add(&_magnitudeFilter, "Magnitude Filter");

    // PRIMARY REQUEST

    // Nov 2012: simplify primary request to have only 1 primary request and then secondary requests (vobsUPDATE_ONLY)
    // cf sclsvrSCENARIO_JSDC 

    // I/280
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_requestI280, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDec, NULL, "&SpType=%5bOBAFGKM%5d*&e_Plx=%3E0.0&Plx=%3E0.999"));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // I/311 to fix Plx / pmRa/Dec (just after ASCC):
    FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // The primary list is completed with the query on catalogs II/225, 
    // I/196, 2MASS, LBSI, II/7A, BSC, SBSC, DENIS

    // DENIS_JK
    FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // 2MASS with K mag filter:
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec, &_filterList, "&opt=%5bTU%5d"));

    /*
     * Note: No LBSI / MERAND requests
     */

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
