/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 *  Definition of sclsvrSCENARIO_JSDC_QUERY class.
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
#include "timlog.h"

/*
 * Local Headers
 */
#include "sclsvrSCENARIO_JSDC_QUERY.h"
#include "sclsvrPrivate.h"

/** Initialize static members */
vobsSTAR_LIST* sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data = NULL;

/**
 * Class constructor
 */
sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY(sdbENTRY* progress) : vobsSCENARIO(progress)
{
}

/**
 * Class destructor
 */
sclsvrSCENARIO_JSDC_QUERY::~sclsvrSCENARIO_JSDC_QUERY()
{
}

/*
 * Public methods
 */

/**
 * Return the name of this scenario
 * @return "JSDC_QUERY"
 */
const char* sclsvrSCENARIO_JSDC_QUERY::GetScenarioName() const
{
    return "JSDC_QUERY";
}

/** preload the JSDC catalog at startup */
void sclsvrSCENARIO_JSDC_QUERY::loadData()
{
    vobsSTAR_LIST* starList = sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data;
    if (starList == NULL)
    {
        // encapsulate the star list in one block to destroy it asap
        mcsSTRING512 fileName;

        // Build the list of star which will come from the virtual observatory
        starList = new vobsSTAR_LIST("JSDC_Data");

        // Load JSDC scenario search results:

        // Define & resolve the file name once:
        strcpy(fileName, sclsvrSCENARIO_JSDC_QUERY_DATA_FILE);

        // Resolve path
        char* resolvedPath = miscResolvePath(fileName);
        if (isNotNull(resolvedPath))
        {
            strcpy(fileName, resolvedPath);
            free(resolvedPath);
        }
        else
        {
            fileName[0] = '\0';
        }
        if (strlen(fileName) != 0)
        {
            logInfo("Loading VO StarList backup: %s", fileName);

            static const char* cmdName = "Load_JSDC";

            // Start timer log
            timlogInfoStart(cmdName);

            if (starList->Load(fileName, NULL, NULL, mcsTRUE) == mcsFAILURE)
            {
                timlogCancel(cmdName);

                // Ignore error (for test only)
                errCloseStack();

                // clear anyway:
                starList->Clear();
            }
            else
            {
                // Stop timer log
                timlogStop(cmdName);
            }
        }
        if (starList->IsEmpty())
        {
            logWarning("Empty JSDC data: disabling scenario [JSDC QUERY]");
            starList = NULL;
        }
        sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data = starList;
    }
}

/** free the JSDC catalog at shutdown */
void sclsvrSCENARIO_JSDC_QUERY::freeData()
{
    vobsSTAR_LIST* starList = sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data;
    if (starList != NULL)
    {
        delete starList;
        sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data = NULL;
    }
}

/**
 * Initialize the JSDC QUERY scenario
 *
 * @param request the user constraint the found stars should conform to
 * @param starList optional input list
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC_QUERY::Init(vobsSCENARIO_RUNTIME &ctx, vobsREQUEST* request, vobsSTAR_LIST* starList)
{
    FAIL_NULL(sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data);

    // Build reference (science) object
    _referenceStar.ClearValues();

    // Add reference star properties
    FAIL(_referenceStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, request->GetObjectRa(), vobsNO_CATALOG_ID));
    FAIL(_referenceStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request->GetObjectDec(), vobsNO_CATALOG_ID));

    // magnitude value
    const char* band = request->GetSearchBand();
    mcsDOUBLE magnitude = request->GetObjectMag();

    mcsSTRING256 magnitudeUcd;
    strcpy(magnitudeUcd, "PHOT_JHN_");
    strcat(magnitudeUcd, band);
    FAIL(_referenceStar.SetPropertyValue(magnitudeUcd, magnitude, vobsNO_CATALOG_ID));


    // Build criteria list on ra dec (given arcsecs) and magnitude range
    mcsDOUBLE deltaRa, deltaDec;

    // Add search geometry constraints:
    if (request->GetSearchAreaGeometry() == vobsBOX)
    {
        // Get search box size (arcmin)
        FAIL(request->GetSearchArea(deltaRa, deltaDec));

        // add 0.5 arcmin for rounding purposes
        deltaRa  += 0.5;
        deltaDec += 0.5;

        // Convert minutes (arcmin) to decimal degrees
        deltaRa  /= (60.0 / 2.0);
        deltaDec /= (60.0 / 2.0);

        logTest("Init: Box search area=[%.3lf %.3lf] arcsec",
                deltaRa  * alxDEG_IN_ARCSEC,
                deltaDec * alxDEG_IN_ARCSEC);
    }
    else
    {
        // Get search radius (arcmin)
        mcsDOUBLE radius;
        FAIL(request->GetSearchArea(radius));

        // add 0.5 arcmin for rounding purposes
        radius += 0.5;

        // Convert minutes (arcmin) to decimal degrees
        radius  /= 60.0;

        deltaRa = deltaDec = radius;

        logTest("Init: Cone search area=[%.3lf] arcsec",
                radius * alxDEG_IN_ARCSEC);
    }

    // Add Criteria on coordinates
    FAIL(_criteriaListRaDecMagRange.Add(vobsSTAR_POS_EQ_RA_MAIN, deltaRa));
    FAIL(_criteriaListRaDecMagRange.Add(vobsSTAR_POS_EQ_DEC_MAIN, deltaDec));

    // Add magnitude criteria
    // TODO: absolute range [min max]
    // not relative diff:
    mcsDOUBLE range = alxMax(fabs(magnitude - request->GetMinMagRange()), fabs(request->GetMaxMagRange() - magnitude));

    logTest("Init: Magnitude %s range=%.2lf", band, range);

    FAIL(_criteriaListRaDecMagRange.Add(magnitudeUcd, range));

    return mcsSUCCESS;
}

/**
 * Execute the scenario
 *
 * The methods execute the scenario which had been loaded before. It will
 * read each entry and query the specific catalog.
 * The scenario execution progress is reported using sdbENTRY instance given to
 * constructor. It writes a message having the following format:
 *      &lt;status&gt; &lt;catalog name&gt; &lt;catalog number&gt;
 *      &lt;number of catalogs&gt;
 * where
 *  <li> &lt;status&gt; is 1 meaning 'In progress'
 *  <li> &lt;catalog name&gt; is the name of catalog currently queried
 *  <li> &lt;catalog number&gt; is the catalog number in the list
 *  <li> &;number of catalogs&gt; is the number of catalogs in the list
 *
 * @param starList vobsSTAR_LIST which is the result of the interrogation,
 * this is the last list return of the last interrogation.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSCENARIO_JSDC_QUERY::Execute(vobsSCENARIO_RUNTIME &ctx, vobsSTAR_LIST &starList)
{
    logInfo("Scenario[%s] Execute() start", GetScenarioName());

    static const char* catalogName = "JSDC_2015.4";

    mcsUINT32 nStep = 0; // step count
    mcsINT64 elapsedTime = 0; // current search time
    mcsINT64 sumSearchTime = 0; // cumulative search time

    // define action for timlog trace
    mcsSTRING256 timLogActionName;

    const char* actionName;

    mcsSTRING32 catalog;
    mcsSTRING256 message;
    mcsSTRING256 logFileName;
    mcsSTRING32 scenarioName;
    mcsSTRING4 step;
    mcsSTRING32 catName;
    char* resolvedPath;


    // Increment step count:
    nStep++;

    // Get action as string:
    actionName = "CONE_SEARCH";


    // **** CATALOG QUERYING ****

    // Get catalog name
    strcpy(catalog, catalogName);
    strcpy(timLogActionName, catalog);

    // Write the current action in the shared database
    snprintf(message, sizeof (message) - 1, "1\t%s\t%d\t%d", catalogName, 1, 1);
    FAIL(_progress->Write(message));

    // Start research in entry's catalog
    logTest("Execute: Step %d - Querying %s [%s] ...", nStep, catalogName, catalogName);

    // define the free pointer flag to avoid double frees (this list and the given list are storing same star pointers):
    starList.SetFreeStarPointers(false);

    vobsSTAR_LIST* catalogStarList = sclsvrSCENARIO_JSDC_QUERY::sclsvrSCENARIO_JSDC_QUERY_Data;

    // Start time counter
    timlogInfoStart(timLogActionName);

    // if research failed, return mcsFAILURE and tempList is empty
    FAIL_DO(catalogStarList->Search(&_referenceStar, &_criteriaListRaDecMagRange, starList),
            timlogCancel(timLogActionName));

    // Stop time counter
    timlogStopTime(timLogActionName, &elapsedTime);
    sumSearchTime += elapsedTime;

    // If the saveSearchList flag is enabled
    // or the verbose level is higher or equal to debug level, search
    // results will be stored in file
    if (_saveSearchList || doLog(logDEBUG))
    {
        // This file will be stored in the $MCSDATA/tmp repository
        strcpy(logFileName, "$MCSDATA/tmp/");
        // Get scenario name, and replace ' ' by '_'
        strcpy(scenarioName, GetScenarioName());
        FAIL(miscReplaceChrByChr(scenarioName, ' ', '_'));
        strcat(logFileName, scenarioName);
        // Add step
        sprintf(step, "%d", nStep);
        strcat(logFileName, "_");
        strcat(logFileName, step);
        // Get catalog name, and replace '/' by '_'
        strcpy(catName, catalogName);
        FAIL(miscReplaceChrByChr(catName, '/', '_'));
        strcat(logFileName, "_");
        strcat(logFileName, catName);
        // Resolve path
        resolvedPath = miscResolvePath(logFileName);
        if (isNotNull(resolvedPath))
        {
            logTest("Execute: Step %d - Save star list to: %s", nStep, resolvedPath);
            // Save resulting list
            FAIL_DO(starList.Save(resolvedPath), free(resolvedPath));
            free(resolvedPath);
        }
    }

    logInfo("Scenario[%s] Execute() done: %d star(s) found.", GetScenarioName(), starList.Size());

    if (sumSearchTime != 0)
    {
        mcsSTRING16 time;
        timlogFormatTime(sumSearchTime, time);
        logInfo("Scenario[%s] total time in catalog queries %s", GetScenarioName(), time);
    }

    return mcsSUCCESS;
}

/*___oOo___*/
