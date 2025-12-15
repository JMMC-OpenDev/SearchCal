/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclsvrGetStarCB class definition.
 */

/*
 * System Headers
 */
#include <iostream>
using namespace std;
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>


/*
 * MCS Headers
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"
#include "timlog.h"
#include "thrd.h"
#include "sdb.h"

/*
 * SCALIB Headers
 */
#include "vobs.h"
extern "C"
{
#include "simcli.h"
}

/*
 * Local Headers
 */
#include "sclsvrVersion.h"
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"
#include "sclsvrErrors.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"


/** maximum number of object identifiers */
#define MAX_OBJECT_IDS  2000

/** (dev) disable file caching */
#define FORCE_NO_CACHE  0

#define PREC            1e-3

/* min e_V values when missing */
#define E_V_MIN         0.01
#define E_V_MIN_MISSING 0.10
#define E_MIN_MISSING   0.25

/* max keep-alive in cache (in seconds) ~ 2 weeks (= 14 days) */
#define CACHE_FILE_MAX_LIFE_SEC (14 * (24 * 3600))

/*
 * Local Macros
 */
#define CMP_DBL(a, b) \
  (!isnan(a) && !isnan(b) && fabs(a - b) < PREC)

#define UPDATE_MAG(property, uX, ue_X)                  \
if (!isnan(uX) || !isnan(ue_X)) {                       \
    mcsDOUBLE val = NAN, err = NAN;                     \
    if (isPropSet(property)) {                          \
        if (isErrorSet(property)) {                     \
            FAIL_TIMLOG_CANCEL(starPtr->GetPropertyValueAndError(property, &val, &err), cmdName); \
        } else {                                        \
            FAIL_TIMLOG_CANCEL(starPtr->GetPropertyValue(property, &val), cmdName); \
        }                                               \
    }                                                   \
    if (isnan(uX)) {                                    \
        uX = val;                                       \
    }                                                   \
    if (!isnan(uX)) {                                   \
        if (isnan(ue_X)) {                              \
           ue_X = !isnan(err) ? err : E_MIN_MISSING;    \
        }                                               \
        if (!CMP_DBL(val, uX) || !CMP_DBL(err, ue_X)) { \
            logInfo("Set property '%s' = %.3lf (%.3lf) (USER)", property->GetName(), uX, ue_X); \
            FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValueAndError(property, uX, ue_X, vobsORIG_USER, vobsCONFIDENCE_MEDIUM, mcsTRUE), cmdName); \
        }                                               \
    }                                                   \
}

#define UPDATE_CMD(property, X, e_X)                            \
{                                                               \
    mcsDOUBLE val = NAN, err = NAN;                             \
    mcsSTRING32 num;                                            \
    cmdPARAM* p;                                                \
    if (isPropSet(property)) {                                  \
        if (isErrorSet(property)) {                             \
            FAIL_TIMLOG_CANCEL(starPtr->GetPropertyValueAndError(property, &val, &err), cmdName); \
        } else {                                                \
            FAIL_TIMLOG_CANCEL(starPtr->GetPropertyValue(property, &val), cmdName); \
        }                                                       \
    }                                                           \
    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam(X, &p), cmdName);    \
    snprintf(num, sizeof(num), "%.3lf", val);                   \
    p->SetUserValue(num);                                       \
    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam(e_X, &p), cmdName);  \
    snprintf(num, sizeof(num), "%.3lf", err);                   \
    p->SetUserValue(num);                                       \
}

/*
 * Public methods
 */

/**
 * Handle GETSTAR command.
 *
 * It handles the given query corresponding to the parameter list of GETSTAR
 * command, processes it and returns the result.
 *
 * @param query string containing request
 * @param dynBuf dynamical buffer where result will be stored
 *
 * @return evhCB_NO_DELETE.
 */
evhCB_COMPL_STAT sclsvrSERVER::GetStarCB(msgMESSAGE &msg, void*)
{
    miscoDYN_BUF dynBuf;
    mcsCOMPL_STAT complStatus = ProcessGetStarCmd(msg.GetBody(), &dynBuf, &msg);

    // Update status to inform request processing is completed
    if (_status.Write("0") == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Check completion status
    if (complStatus == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    return evhCB_NO_DELETE;
}

/**
 * Handle GETSTAR command.
 *
 * It handles the given query corresponding to the parameter list of GETSTAR
 * command, processes it and returns the result.
 *
 * @param query string containing request
 * @param dynBuf dynamical buffer where result will be stored
 *
 * @return evhCB_NO_DELETE.
 */
mcsCOMPL_STAT sclsvrSERVER::GetStar(const char* query, miscoDYN_BUF* dynBuf)
{
    // Get calibrators
    mcsCOMPL_STAT complStatus = ProcessGetStarCmd(query, dynBuf, NULL);

    // Update status to inform request processing is completed
    FAIL(_status.Write("0"));

    return complStatus;
}

/**
 * GETSTAR command processing method.
 *
 * This method is called by GETSTAR command callback. It selects appropriated
 * scenario, executes it and returns resulting list of calibrators
 *
 * @param query user query containing all command parameters in string format
 * @param dynBuf dynamical buffer where star data will be stored
 * @param msg message corresponding to the received command. If not NULL, a
 * thread is started and intermediate replies are sent giving the request
 * processing status.
 *
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSERVER::ProcessGetStarCmd(const char* query,
                                              miscoDYN_BUF* dynBuf,
                                              msgMESSAGE* msg = NULL)
{
    static const char* cmdName = "GETSTAR";

    // Get the request as a string for the case of Save in VOTable
    mcsSTRING1024 requestString;
    strncpy(requestString, query, sizeof (requestString) - 1);

    // Search command
    sclsvrGETSTAR_CMD getStarCmd(cmdName, query);

    // Parse command
    FAIL(getStarCmd.Parse());

    // Start timer log
    timlogInfoStart(cmdName);

    // Monitoring task
    thrdTHREAD_STRUCT monitorTask;

    // If request comes from msgMESSAGE, start monitoring task send send
    // request progression status
    if (IS_NOT_NULL(msg))
    {
        // Monitoring task parameters
        sclsvrMONITOR_TASK_PARAMS monitorTaskParams;
        monitorTaskParams.server = this;
        monitorTaskParams.message = msg;
        monitorTaskParams.status = &_status;

        // Monitoring task
        monitorTask.function = sclsvrMonitorTask;
        monitorTask.parameter = (thrdFCT_ARG*) & monitorTaskParams;

        // Launch the thread only if SDB had been successfully started
        FAIL_TIMLOG_CANCEL(thrdThreadCreate(&monitorTask), cmdName);
    }

    if (IS_NOT_NULL(dynBuf))
    {
        FAIL_TIMLOG_CANCEL(dynBuf->Reset(), cmdName);
    }


    mcsLOGICAL diagnoseFlag = mcsFALSE;
    FAIL_TIMLOG_CANCEL(getStarCmd.GetDiagnose(&diagnoseFlag), cmdName);

    char* objectNames = NULL;
    FAIL_TIMLOG_CANCEL(getStarCmd.GetObjectName(&objectNames), cmdName);

    // Parse objectName to get multiple star identifiers (separated by comma)
    mcsUINT32    nbObjects = 0;
    mcsSTRING256 objectIds[MAX_OBJECT_IDS];

    /* TODO: define a new getStarCmd parameter for the name separator */
    // JMDC 2020.07 has ',' in "WDSJ05167+4600Aa,Ab" !!
    const char delimiter = (strchr(objectNames, '|') != NULL) ? '|' : ',';
    logDebug("delimiter: '%c'", delimiter);

    /* may fail: too many identifiers */
    FAIL_TIMLOG_CANCEL(miscSplitString(objectNames, delimiter, objectIds, MAX_OBJECT_IDS, &nbObjects), cmdName);


    const bool isRegressionTest = IS_FALSE(logGetPrintFileLine());
    /* if multiple objects, disable log */
    const bool diagnose = (nbObjects <= 1) && (IS_TRUE(diagnoseFlag) || alxIsDevFlag());

    /* Enable log thread context if not in regression test mode (-noFileLine) */
    if (diagnose && !isRegressionTest)
    {
        logEnableThreadContext();
    }

    // all following logs are returned to the user (log thread context):
    logInfo("diagnose mode:   %d", diagnose);
    logInfo("objectNames:    '%s'", objectNames);
    logInfo("nbObjects:       %d", nbObjects);


    mcsDOUBLE wlen;
    mcsDOUBLE baseMax;
    char* outputFormat = NULL;
    char* file = NULL;
    mcsLOGICAL enableScenario = mcsFALSE;

    FAIL_TIMLOG_CANCEL(getStarCmd.GetWlen(&wlen), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetBaseMax(&baseMax), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetFormat(&outputFormat), cmdName);

    if (IS_TRUE(getStarCmd.IsDefinedFile()))
    {
        FAIL_TIMLOG_CANCEL(getStarCmd.GetFile(&file), cmdName);
    }

    if (IS_FALSE(getStarCmd.IsDefinedScenario()))
    {
        cmdPARAM* p;
        if (getStarCmd.GetParam("scenario", &p) == mcsSUCCESS)
        {
            // enable scenario if the parameter is missing (former CLI behaviour):
            p->SetUserValue("true");
        }
    }
    // anyway get scenario flag:
    FAIL_TIMLOG_CANCEL(getStarCmd.GetScenario(&enableScenario), cmdName);
    logInfo("enable scenario: %d", enableScenario);


    // Get advanced parameter for a single object:
    mcsLOGICAL forceUpdate = mcsFALSE;
    mcsDOUBLE uV, ue_V;
    mcsDOUBLE uJ, ue_J;
    mcsDOUBLE uH, ue_H;
    mcsDOUBLE uK, ue_K;
    char*     uSpType = NULL;

    if (nbObjects == 1)
    {
        if (getStarCmd.IsDefinedForceUpdate())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetForceUpdate(&forceUpdate), cmdName);
        }

        uV = NAN;
        ue_V = NAN;
        if (getStarCmd.IsDefinedV())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetV(&uV), cmdName);
        }
        if (getStarCmd.IsDefinedE_V())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_V(&ue_V), cmdName);
        }

        uJ = NAN;
        ue_J = NAN;
        if (getStarCmd.IsDefinedJ())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetJ(&uJ), cmdName);
        }
        if (getStarCmd.IsDefinedE_J())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_J(&ue_J), cmdName);
        }

        uH = NAN;
        ue_H = NAN;
        if (getStarCmd.IsDefinedH())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetH(&uH), cmdName);
        }
        if (getStarCmd.IsDefinedE_H())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_H(&ue_H), cmdName);
        }

        uK = NAN;
        ue_K = NAN;
        if (getStarCmd.IsDefinedK())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetK(&uK), cmdName);
        }
        if (getStarCmd.IsDefinedE_K())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_K(&ue_K), cmdName);
        }

        if (getStarCmd.IsDefinedSP_TYPE())
        {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetSP_TYPE(&uSpType), cmdName);
        }
    }
    // Ignore forceUpdate if scenario = false:
    if (IS_FALSE(enableScenario))
    {
        cmdPARAM* p;
        if (getStarCmd.GetParam("forceUpdate", &p) == mcsSUCCESS)
        {
            // disable forceUpdate:
            p->SetUserValue("false");
        }
        FAIL_TIMLOG_CANCEL(getStarCmd.GetForceUpdate(&forceUpdate), cmdName);
    }
    logInfo("force update:    %d", forceUpdate);


    // Reuse scenario results for GetStar:
    _useVOStarListBackup = (FORCE_NO_CACHE) ? false : true;
    mcsSTRING512 fileName;


    vobsSTAR_LIST starList("GetStar");

    // Build the list of calibrator (final output)
    sclsvrCALIBRATOR_LIST calibratorList("Calibrators");


    for (mcsUINT32 i = 0; i < nbObjects; i++)
    {
        char* objectId = objectIds[i];

        // Remove each token trailing and leading white spaces
        miscTrimString(objectId, " \t\n");

        logDebug("objectId: %s", objectId);

        if (strlen(objectId) == 0)
        {
            // skip empty identifier
            continue;
        }

        // Get the star position from SIMBAD
        mcsSTRING32 ra, dec;
        mcsDOUBLE pmRa, pmDec;
        mcsDOUBLE plx, ePlx;
        mcsDOUBLE sMagV, sEMagV;
        mcsSTRING64 spType, mainId;
        mcsSTRING256 objTypes;

        // TODO: skip simbad = cone search (ra/dec, pmRa/pmDe, mainId), plx ?

        if (simcliGetCoordinates(objectId, ra, dec, &pmRa, &pmDec,
                                 &plx, &ePlx, &sMagV, &sEMagV,
                                 spType, objTypes, mainId) == mcsFAILURE)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, objectId, "CDS SIMBAD");
                TIMLOG_CANCEL(cmdName);
            }
            else
            {
                logInfo("Star '%.80s' has not been found in CDS SIMBAD", objectId);
                // skip object:
                continue;
            }
        }
        logInfo("GetStar[%s]: RA/DEC='%s %s' pmRA/pmDEC=(%.1lf %.1lf) plx=%.1lf(%.1lf) V=%.2lf(%.2lf) spType='%s' objTypes='%s' mainID='%s'",
                objectId, ra, dec, pmRa, pmDec, plx, ePlx, sMagV, sEMagV, spType, objTypes, mainId);


        // Prepare request to search information in other catalog
        sclsvrREQUEST request;
        FAIL_TIMLOG_CANCEL(request.SetObjectName(mainId), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetObjectRa(ra), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetObjectDec(dec), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetPmRa(pmRa), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetPmDec(pmDec), cmdName);
        // Affect the file name
        if (IS_NOT_NULL(file))
        {
            FAIL_TIMLOG_CANCEL(request.SetFileName(file), cmdName);
        }
        FAIL_TIMLOG_CANCEL(request.SetSearchBand("K"), cmdName);

        // Use the JSDC Catalog Query Scenario (faint)
        FAIL_TIMLOG_CANCEL(request.SetBrightFlag(mcsFALSE), cmdName);


        // clear anyway:
        starList.Clear();

        // Try searching in JSDC (loaded):
        if (!forceUpdate && starList.IsEmpty() && IsQueryJSDCFaint())
        {
            // 2 arcsec to match Star(s) (identifier check):
            mcsDOUBLE filterRadius = (mcsDOUBLE) (2.0 * alxARCSEC_IN_DEGREES);
            FAIL_TIMLOG_CANCEL(request.SetSearchArea(filterRadius * alxDEG_IN_ARCMIN), cmdName);

            // init the scenario
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Init(&_scenarioJSDC_Query, &request, &starList), cmdName);

            // Note: do not modify vobsSTAR instance shared in JSDC cache retrieved by this query:
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Search(&_scenarioJSDC_Query, starList), cmdName);

            mcsUINT32 nStars = starList.Size();

            if (nStars > 1)
            {
                logInfo("GetStar: too many results (%d) from JSDC", nStars);
                starList.Clear();
            }
        }

        // Load previous scenario search results:
        if (!forceUpdate && starList.IsEmpty() && _useVOStarListBackup)
        {
            /* 
             * Replace invalid characters by '_' as SIMBAD MAIN_ID can contain following ascii characters:
             * "\0","#","'","(",")","*","+","-",".","/",":","?","[","]","_",
             * "0","1","2","3","4","5","6","7","8","9",
             * "a","A","b","B","c","C","d","D","e","E","f","F","g","G","h","H",
             * "i","I","j","J","k","K","l","L","m","M","n","N","o","O","p","P",
             * "q","Q","r","R","s","S","t","T","u","U","v","V","w","W","x","X",
             * "y","Y","z","Z"
             */
            mcsSTRING128 cleanObjName;
            strncpy(cleanObjName, request.GetObjectName(), sizeof (cleanObjName) - 1);
            FAIL_TIMLOG_CANCEL(miscReplaceNonAlphaNumericChrByChr(cleanObjName, '-'), cmdName);

            // Define & resolve the file name once:
            strcpy(fileName, "$MCSDATA/tmp/GetStar/SearchListBackup_");
            strcat(fileName, _scenarioSingleStar.GetScenarioName());
            strcat(fileName, "_");
            strcat(fileName, cleanObjName);
            strcat(fileName, "_");
            strcat(fileName, request.GetObjectRa());
            strcat(fileName, "_");
            strcat(fileName, request.GetObjectDec());
            strcat(fileName, ".dat");

            FAIL_TIMLOG_CANCEL(miscReplaceChrByChr(fileName, ' ', '_'), cmdName);

            // Resolve path
            char* resolvedPath = miscResolvePath(fileName);
            if (IS_NOT_NULL(resolvedPath))
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
                struct stat stats;

                /* Test if the cache file exists? */
                if ((stat(fileName, &stats) == 0) && (stats.st_size > 0L))
                {
                    logDebug("StarList file size: %d", stats.st_size);

                    struct timeval time;
                    struct tm now, fileMod;

                    /* Get local time */
                    gettimeofday(&time, NULL);

                    /* Use GMT date (up to second precision) */
                    gmtime_r(&time.tv_sec, &now);
                    gmtime_r(&stats.st_mtime, &fileMod);

                    logTest("StarList file modified on: %d-%02d-%02d %02d:%02d:%02d",
                            fileMod.tm_year + 1900, fileMod.tm_mon + 1, fileMod.tm_mday,
                            fileMod.tm_hour, fileMod.tm_min, fileMod.tm_sec);

                    mcsDOUBLE elapsed = (mcsDOUBLE) difftime(mktime(&now), mktime(&fileMod));
                    logDebug("StarList file age: %.1lf seconds", elapsed);


                    if (IS_FALSE(enableScenario) || ((elapsed >= 0) && (elapsed < CACHE_FILE_MAX_LIFE_SEC)))
                    {
                        logInfo("Loading StarList backup: %s", fileName);

                        if (starList.Load(fileName, NULL, NULL, mcsTRUE) == mcsFAILURE)
                        {
                            // Ignore error (for test only)
                            errCloseStack();

                            // clear anyway:
                            starList.Clear();
                        }
                    }
                    else
                    {
                        logInfo("Skip loading StarList backup: %s (age = %.1lf s)", fileName, elapsed);
                    }
                }
            }
        }

        // Always check from cache (local or JSDC):
        mcsUINT32 nStars = starList.Size();

        if (nStars == 1)
        {
            // check ID SIMBAD ?
            vobsSTAR* star = starList.GetNextStar(mcsTRUE);

            if (IS_NOT_NULL(star))
            {
                vobsSTAR_PROPERTY* property = star->GetProperty(vobsSTAR_ID_SIMBAD);
                if (isPropSet(property))
                {
                    mcsSTRING64 starSimbadId, requestSimbadId;
                    strncpy(starSimbadId, property->GetValue(), sizeof (starSimbadId) - 1);
                    strncpy(requestSimbadId, mainId, sizeof (requestSimbadId) - 1);
                    // remove space characters
                    miscDeleteChr((char *) starSimbadId, ' ', mcsTRUE);
                    miscDeleteChr((char *) requestSimbadId, ' ', mcsTRUE);

                    logTest("Found star [%s] for SIMBAD ID [%s]", starSimbadId, requestSimbadId);

                    // check Simbad Main ID (exact):
                    if (strcmp(starSimbadId, requestSimbadId) != 0)
                    {
                        logWarning("Mismatch identifiers: [%s] vs [%s]", starSimbadId, requestSimbadId);
                        starList.Clear();
                    }
                }
            }
        }

        if (starList.IsEmpty())
        {
            if (enableScenario)
            {
                logInfo("Performing GetStar scenario for '%s' ...", mainId);

                // Set the reference star:
                vobsSTAR refStar;
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  request.GetObjectRa(),  vobsCATALOG_SIMBAD_ID), cmdName);
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request.GetObjectDec(), vobsCATALOG_SIMBAD_ID), cmdName);

                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_POS_EQ_PMRA,     request.GetPmRa(),  vobsCATALOG_SIMBAD_ID), cmdName);
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_POS_EQ_PMDEC,    request.GetPmDec(), vobsCATALOG_SIMBAD_ID), cmdName);

                if (!isnan(plx))
                {
                    FAIL_TIMLOG_CANCEL(refStar.SetPropertyValueAndError(vobsSTAR_POS_PARLX_TRIG, plx, ePlx, vobsCATALOG_SIMBAD_ID), cmdName);
                }

                // Define SIMBAD SP_TYPE, OBJ_TYPES and main identifier (easier crossmatch):
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_SPECT_TYPE_MK,   spType, vobsCATALOG_SIMBAD_ID), cmdName);
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_OBJ_TYPES,     objTypes, vobsCATALOG_SIMBAD_ID), cmdName);
                FAIL_TIMLOG_CANCEL(refStar.SetPropertyValue(vobsSTAR_ID_SIMBAD,       mainId, vobsCATALOG_SIMBAD_ID), cmdName);

                // note: how to set flux V (from SIMBAD if not in ASCC) but before the GAIA query (matching V range) ? */
                starList.AddAtTail(refStar);

                // init the scenario
                FAIL_TIMLOG_CANCEL(_virtualObservatory.Init(&_scenarioSingleStar, &request, &starList), cmdName);

                FAIL_TIMLOG_CANCEL(_virtualObservatory.Search(&_scenarioSingleStar, starList), cmdName);

                // Save the current scenario search results:
                if (_useVOStarListBackup)
                {
                    if (strlen(fileName) != 0)
                    {
                        logInfo("Saving current StarList: %s", fileName);

                        if (starList.Save(fileName, mcsTRUE) == mcsFAILURE)
                        {
                            // Ignore error (for test only)
                            errCloseStack();
                        }
                    }
                }
            }
            else
            {
                logInfo("Skipping GetStar scenario for '%s' (disabled).", mainId);
            }
        }

        // If the star has not been found in catalogs (single star):

        const char* provenance = (IS_TRUE(enableScenario) ? "CDS VizieR catalogs" : "JSDC (scenario disabled)");

        nStars = starList.Size();
        if (nStars == 0)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, mainId, provenance);
            }
            else
            {
                logInfo("Star '%.80s' has not been found in %s", mainId, provenance);
            }
        }
        else if (nStars > 1)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, mainId, provenance);
            }
            else
            {
                logInfo("Star '%.80s' has too many results in %s", mainId, provenance);
            }
        }
        else
        {
            // nStars == 1:

            // Get first star of the list:
            vobsSTAR* starPtr = starList.GetNextStar(mcsTRUE);

            // Note: copy star before modifying the vobsSTAR instance shared in JSDC cache:
            if (!starList.IsFreeStarPointers())
            {
                starPtr = new vobsSTAR(*starPtr);
            }

            // Set queried identifier in the Target_ID column (= given user's object id):
            /* note: it is cleared by scenario (fill it after scenario execution) */
            starPtr->GetTargetIdProperty()->SetValue(objectId, vobsORIG_USER);

            // Fix missing parallax with latest SIMBAD information:
            if (!starPtr->IsPropertySet(vobsSTAR_POS_PARLX_TRIG) && !isnan(plx))
            {
                logInfo("Set property '%s' = %.3lf (%.3lf) (SIMBAD)", vobsSTAR_POS_PARLX_TRIG, plx, ePlx);
                FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValueAndError(vobsSTAR_POS_PARLX_TRIG, plx, ePlx, vobsCATALOG_SIMBAD_ID), cmdName);
            }

            // Update SIMBAD SP_TYPE, OBJ_TYPES and main identifier (easier crossmatch):
            logInfo("Set property '%s' = '%s' (SIMBAD)", vobsSTAR_SPECT_TYPE_MK, spType);
            FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, spType, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE), cmdName);

            logInfo("Set property '%s' = '%s' (SIMBAD)", vobsSTAR_OBJ_TYPES, objTypes);
            FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValue(vobsSTAR_OBJ_TYPES, objTypes, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE), cmdName);

            logInfo("Set property '%s' = '%s' (SIMBAD)", vobsSTAR_ID_SIMBAD, mainId);
            FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValue(vobsSTAR_ID_SIMBAD, mainId, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE), cmdName);

            /* Set flux V (from SIMBAD) */
            vobsSTAR_PROPERTY* mVProperty_SIMBAD = starPtr->GetProperty(vobsSTAR_PHOT_SIMBAD_V);
            FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValueAndError(mVProperty_SIMBAD, sMagV, sEMagV, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_MEDIUM), cmdName);

            // Fix missing V mag with SIMBAD or GAIA information (now to get accurate information in the web form):
            FAIL_TIMLOG_CANCEL(starPtr->UpdateMissingMagV(), cmdName);

            if (nbObjects == 1)
            {
                // overwrite all fields given by GetStar parameters used by the diameter estimation
                // VJHK + errors + SPTYPE and allow user correction of catalog values in the web form (2nd step)

                vobsSTAR_PROPERTY* mVProperty = starPtr->GetProperty(vobsSTAR_PHOT_JHN_V);
                vobsSTAR_PROPERTY* mJProperty = starPtr->GetProperty(vobsSTAR_PHOT_JHN_J);
                vobsSTAR_PROPERTY* mHProperty = starPtr->GetProperty(vobsSTAR_PHOT_JHN_H);
                vobsSTAR_PROPERTY* mKProperty = starPtr->GetProperty(vobsSTAR_PHOT_JHN_K);
                vobsSTAR_PROPERTY* spProperty = starPtr->GetProperty(vobsSTAR_SPECT_TYPE_MK);

                UPDATE_MAG(mVProperty, uV, ue_V);
                UPDATE_MAG(mJProperty, uJ, ue_J);
                UPDATE_MAG(mHProperty, uH, ue_H);
                UPDATE_MAG(mKProperty, uK, ue_K);

                if (!IS_STR_EMPTY(uSpType))
                {
                    const char* val = (isPropSet(spProperty)) ? starPtr->GetPropertyValue(vobsSTAR_SPECT_TYPE_MK) : NULL;
                    if (IS_STR_EMPTY(val) || strcmp(val, uSpType) != 0)
                    {
                        logInfo("Set property '%s' = '%s' (USER)", vobsSTAR_SPECT_TYPE_MK, spType);
                        FAIL_TIMLOG_CANCEL(starPtr->SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, uSpType, vobsORIG_USER, vobsCONFIDENCE_MEDIUM, mcsTRUE), cmdName);
                    }
                }

                // Update GetStarCmd parameters with used values:
                UPDATE_CMD(mVProperty, "V", "e_V")
                UPDATE_CMD(mJProperty, "J", "e_J")
                UPDATE_CMD(mHProperty, "H", "e_H")
                UPDATE_CMD(mKProperty, "K", "e_K")

                if (isPropSet(spProperty))
                {
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("SP_TYPE", &p), cmdName);
                    p->SetUserValue(starPtr->GetPropertyValue(spProperty));
                }
            }

            // Add a new calibrator in the list of calibrator (final output)
            calibratorList.AddAtTail(*starPtr);

            // Note: free star cloned from JSDC cache:
            if (!starList.IsFreeStarPointers())
            {
                delete starPtr;
            }
        }

    } /* iterate on objectIds */

    // clear anyway:
    starList.Clear();

    // If stars have been found in catalogs
    if (calibratorList.Size() == 0)
    {
        TIMLOG_CANCEL(cmdName);
    }
    else
    {
        // Prepare request to perform computations
        sclsvrREQUEST request;

        /* set diagnose flag */
        FAIL_TIMLOG_CANCEL(request.SetDiagnose(diagnoseFlag), cmdName);

        /* use all object names */
        FAIL_TIMLOG_CANCEL(request.SetObjectName(objectNames), cmdName);
        // Do not set Object Ra/Dec to skip distance computation (and sort)
        // Affect the file name
        if (IS_NOT_NULL(file))
        {
            FAIL_TIMLOG_CANCEL(request.SetFileName(file), cmdName);
        }
        FAIL_TIMLOG_CANCEL(request.SetSearchBand("K"), cmdName);

        // Use the JSDC Catalog Query Scenario (faint)
        FAIL_TIMLOG_CANCEL(request.SetBrightFlag(mcsFALSE), cmdName);

        // Optional parameters for ComputeVisibility()
        FAIL_TIMLOG_CANCEL(request.SetObservingWlen(wlen), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetMaxBaselineLength(baseMax), cmdName);


        // Complete the calibrators list:
        FAIL_TIMLOG_CANCEL(calibratorList.Complete(request), cmdName);


        // Pack the list result in a buffer in order to send it
        string xmlOutput;
        xmlOutput.reserve(2048);

        // use getStarCmd directly as GetCalCmd <> GetStarCmd:
        getStarCmd.AppendParamsToVOTable(xmlOutput);

        const char* command  = "GetStar";
        const char* header = "GetStar software (In case of problem, please report to jmmc-user-support@jmmc.fr)";

        // Disable trimming columns:
        vobsTRIM_COLUMN_MODE trimColumnMode = vobsTRIM_COLUMN_OFF;

        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof (softwareVersion) - 1, "%s v%s", "SearchCal Server", sclsvrVERSION);

        // Get the thread's log:
        const char* tlsLog = logContextGetBuffer();

        // If a filename has been given, store results as file
        if (strlen(request.GetFileName()) != 0)
        {
            mcsSTRING32 fileName;
            strcpy(fileName, request.GetFileName());

            // If the extension is .vot, save as VO table
            if ((strcmp(outputFormat, "vot") == 0) && (strcmp(miscGetExtension(fileName), "vot") == 0))
            {
                // Save the list as a VOTable (DO NOT trim columns)
                FAIL_TIMLOG_CANCEL(calibratorList.SaveToVOTable(command, request.GetFileName(), header, softwareVersion,
                                                                requestString, xmlOutput.c_str(), trimColumnMode, tlsLog), cmdName);
            }
            else
            {
                // Save the list as a TSV file
                FAIL_TIMLOG_CANCEL(calibratorList.SaveTSV(request.GetFileName(), header, softwareVersion, requestString), cmdName);
            }
        }

        // Give back CDATA for msgMESSAGE reply.
        if (IS_NOT_NULL(dynBuf))
        {
            if (IS_NOT_NULL(msg))
            {
                calibratorList.Pack(dynBuf);
            }
            else
            {
                if (strcmp(outputFormat, "vot") == 0)
                {
                    // Give back a VOTable (DO NOT trim columns)
                    FAIL_TIMLOG_CANCEL(calibratorList.GetVOTable(command, header, softwareVersion, requestString, xmlOutput.c_str(),
                                                                 dynBuf, trimColumnMode, tlsLog), cmdName);
                }
                else
                {
                    // Otherwise, give back a TSV file
                    FAIL_TIMLOG_CANCEL(calibratorList.GetTSV(header, softwareVersion, requestString, dynBuf), cmdName);
                }
            }
        }
    }

    if (IS_NOT_NULL(msg))
    {
        // Wait for the actionForwarder thread end
        FAIL_TIMLOG_CANCEL(thrdThreadWait(&monitorTask), cmdName);
    }

    // Stop timer log
    timlogStop(cmdName);

    return mcsSUCCESS;
}

/*___oOo___*/
