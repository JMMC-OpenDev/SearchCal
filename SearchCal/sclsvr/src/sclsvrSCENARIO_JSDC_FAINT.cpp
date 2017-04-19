/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_JSDC_FAINT class.
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
#include "sclsvrSCENARIO_JSDC_FAINT.h"
#include "sclsvrPrivate.h"

/**
 * Class constructor
 */
sclsvrSCENARIO_JSDC_FAINT::sclsvrSCENARIO_JSDC_FAINT(sdbENTRY* progress) : vobsSCENARIO(progress),
_starList("Main")
{
    // Save the xml output (last chunk)
    _saveSearchXml = mcsTRUE;
    // Load and Save intermediate results
    _loadSearchList = true;
    _saveSearchList = true;
    // disable duplicates removal in latest SIMBAD x ASCC (2017.4):
    _removeDuplicates = false;
}

/**
 * Class destructor
 */
sclsvrSCENARIO_JSDC_FAINT::~sclsvrSCENARIO_JSDC_FAINT()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC_FAINT"
 */
const char* sclsvrSCENARIO_JSDC_FAINT::GetScenarioName() const
{
    return "JSDC_FAINT";
}

/**
 * Initialize the JSDC FAINT scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC_FAINT::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    // Clear the list input and list output which will be used
    _starList.Clear();

    // BUILD REQUEST USED
    _request.Copy(*request);

    // BUILD CRITERIA LIST
    FAIL(InitCriteriaLists());

    ////////////////////////////////////////////////////////////////////////
    // PRIMARY REQUEST on LOCAL CATALOG
    ////////////////////////////////////////////////////////////////////////

    // Get only RA/Dec (J2000 - epoch 2000) + pmRa/Dec + optional SpType/ObjType
    FAIL(AddEntry(vobsCATALOG_JSDC_FAINT_LOCAL_ID, &_request, NULL, &_starList, vobsCLEAR_MERGE, &_criteriaListRaDecJSDC));

    // Merge with I/280 to get all catalog properties
    FAIL(AddEntry(vobsCATALOG_ASCC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    ////////////////////////////////////////////////////////////////////////
    // SECONDARY REQUEST
    ////////////////////////////////////////////////////////////////////////

    // I/311 - Hipparcos, the New Reduction (van Leeuwen, 2007)
    // to fix Plx / pmRa/Dec (just after ASCC):
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_HIP2_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // HIP1 - V / B / Ic (2013-04-18)
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_HIP1_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // 2MASS
    FAIL(AddEntry(vobsCATALOG_MASS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec2MASS));

    // II/328/allwise aka WISE (LMN)
    FAIL(AddEntry(vobsCATALOG_WISE_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecAkari));

    // DENIS_JK - J-K DENIS photometry of bright southern stars (Kimeswenger+ 2004)
    // FAIL(AddEntry(vobsCATALOG_DENIS_JK_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/7A - UBVRIJKLMNH Photoelectric Catalogue
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_PHOTO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecMagV));

    // II/225 - Catalog of Infrared Observations, Edition 5 (Gezari+ 1999)
    // FAIL(AddEntry(vobsCATALOG_CIO_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // I/196
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_HIC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // BSC - Bright Star Catalogue, 5th Revised Ed. (Hoffleit+, 1991)
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_BSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    // SBSC - Supplement to the Bright Star Catalogue (Hoffleit+ 1983)
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_SBSC_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecHd));

    if (vobsCATALOG_DENIS_ID_ENABLE)
    {
        // B/denis - so far not able to use Denis properly
        FAIL(AddEntry(vobsCATALOG_DENIS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));
    }

    // B/sb9 - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_SB9_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // B/wds/wds - 9th Catalogue of Spectroscopic Binary Orbits (Pourbaix+ 2004-2013)
    FAIL(AddEntry(vobsCATALOG_WDS_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDec));

    // II/297/irc aka AKARI
    // FAINT: disabled
    // FAIL(AddEntry(vobsCATALOG_AKARI_ID, &_request, &_starList, &_starList, vobsUPDATE_ONLY, &_criteriaListRaDecAkari));

    return mcsSUCCESS;
}


/*___oOo___*/
