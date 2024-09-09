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

#define FORCE_NO_CACHE  0

/*
 * Local Macros
 */

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
        dynBuf->Reset();
    }

    // Get filename
    char* file = NULL;
    if (IS_TRUE(getStarCmd.IsDefinedFile()))
    {
        FAIL_TIMLOG_CANCEL(getStarCmd.GetFile(&file), cmdName);
    }

    mcsDOUBLE wlen;
    mcsDOUBLE baseMax;
    char* objectName = NULL;
    char* outputFormat = NULL;
    mcsLOGICAL diagnoseFlag = mcsFALSE;

    FAIL_TIMLOG_CANCEL(getStarCmd.GetWlen(&wlen), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetBaseMax(&baseMax), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetObjectName(&objectName), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetFormat(&outputFormat), cmdName);
    FAIL_TIMLOG_CANCEL(getStarCmd.GetDiagnose(&diagnoseFlag), cmdName);

    // Parse objectName to get multiple star identifiers (separated by comma)
    mcsUINT32    nbObjects = 0;
    mcsSTRING256 objectIds[MAX_OBJECT_IDS];

    logInfo("objectNames: '%s'", objectName);

    /* TODO: define a new getStarCmd parameter for the name separator */
    // JMDC 2020.07 has ',' in "WDSJ05167+4600Aa,Ab" !!
    const char delimiter = (strchr(objectName, '|') != NULL) ? '|' : ',';
    logDebug("delimiter: '%c'", delimiter);

    /* may fail: too many identifiers */
    FAIL_TIMLOG_CANCEL(miscSplitString(objectName, delimiter, objectIds, MAX_OBJECT_IDS, &nbObjects), cmdName);
    logInfo("nbObjects: %d", nbObjects);


    const bool isRegressionTest = IS_FALSE(logGetPrintFileLine());
    /* if multiple objects, disable log */
    const bool diagnose = (nbObjects <= 1) && (IS_TRUE(diagnoseFlag) || alxIsDevFlag());

    if (diagnose)
    {
        logInfo("diagnose mode enabled.");
    }

    mcsDOUBLE uV, ue_V;
    mcsDOUBLE uJ, ue_J;
    mcsDOUBLE uH, ue_H;
    mcsDOUBLE uK, ue_K;
    char*     uSpType = NULL;
    
    if (nbObjects == 1)
    {
        uV = NAN; ue_V = NAN;
        if (getStarCmd.IsDefinedV()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetV(&uV), cmdName);
        }
        if (getStarCmd.IsDefinedE_V()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_V(&ue_V), cmdName);
        }

        uJ = NAN; ue_J = NAN;
        if (getStarCmd.IsDefinedJ()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetJ(&uJ), cmdName);
        }
        if (getStarCmd.IsDefinedE_J()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_J(&ue_J), cmdName);
        }

        uH = NAN; ue_H = NAN;
        if (getStarCmd.IsDefinedH()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetH(&uH), cmdName);
        }
        if (getStarCmd.IsDefinedE_H()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_H(&ue_H), cmdName);
        }

        uK = NAN; ue_K = NAN;
        if (getStarCmd.IsDefinedK()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetK(&uK), cmdName);
        }
        if (getStarCmd.IsDefinedE_K()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetE_K(&ue_K), cmdName);
        }
        
        if (getStarCmd.IsDefinedSP_TYPE()) {
            FAIL_TIMLOG_CANCEL(getStarCmd.GetSP_TYPE(&uSpType), cmdName);
        }
    }
    
    /* Enable log thread context if not in regression test mode (-noFileLine) */
    if (diagnose && !isRegressionTest)
    {
        logEnableThreadContext();
    }


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
        mcsDOUBLE magV, eMagV;
        mcsSTRING64 spType, objTypes, mainId;

        if (simcliGetCoordinates(objectId, ra, dec, &pmRa, &pmDec, 
                                 &plx, &ePlx, &magV, &eMagV, 
                                 spType, objTypes, mainId) == mcsFAILURE)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, objectId, "SIMBAD");
                TIMLOG_CANCEL(cmdName);
            }
            else
            {
                logInfo("Star '%.80s' has not been found in SIMBAD", objectId);
                continue;
            }
        }

        logInfo("GetStar[%s]: RA/DEC='%s %s' pmRA/pmDEC=(%.1lf %.1lf) plx=%.1lf(%.1lf) V=%.1lf(%.1lf) spType='%s' objTypes='%s' IDS='%s'",
                objectId, ra, dec, pmRa, pmDec, plx, ePlx, magV, eMagV, spType, objTypes, mainId);
        
        // Prepare request to search information in other catalog
        sclsvrREQUEST request;
        FAIL_TIMLOG_CANCEL(request.SetObjectName(mainId), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetObjectRa(ra), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetObjectDec(dec), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetPmRa((mcsDOUBLE) pmRa), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetPmDec((mcsDOUBLE) pmDec), cmdName);
        // Affect the file name
        if (IS_NOT_NULL(file))
        {
            FAIL_TIMLOG_CANCEL(request.SetFileName(file), cmdName);
        }
        FAIL_TIMLOG_CANCEL(request.SetSearchBand("K"), cmdName);


        // clear anyway:
        starList.Clear();

        // Try searching in JSDC:
        if (starList.IsEmpty() && sclsvrSERVER_queryJSDC_Faint)
        {
            // Use the JSDC Catalog Query Scenario (faint)
            request.SetBrightFlag(mcsFALSE);

            // 2 arcsec to match Star(s) (identifier check):
            mcsDOUBLE filterRadius = (mcsDOUBLE) (2.0 * alxARCSEC_IN_DEGREES);

            request.SetSearchArea(filterRadius * alxDEG_IN_ARCMIN);

            // init the scenario
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Init(&_scenarioJSDC_Query, &request, &starList), cmdName);

            FAIL_TIMLOG_CANCEL(_virtualObservatory.Search(&_scenarioJSDC_Query, starList), cmdName);

            mcsUINT32 nStars = starList.Size();

            if (nStars > 1)
            {
                logInfo("GetStar: too many results (%d) from JSDC", nStars);
                starList.Clear();
            }
        }

        // Load previous scenario search results:
        if (starList.IsEmpty() && _useVOStarListBackup)
        {
            // Define & resolve the file name once:
            strcpy(fileName, "$MCSDATA/tmp/GetStar/SearchListBackup_");
            strcat(fileName, _scenarioSingleStar.GetScenarioName());
            strcat(fileName, "_");
            strcat(fileName, request.GetObjectName());
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
                logInfo("Loading VO StarList backup: %s", fileName);

                // TODO: check file age (only load if less than 1 week/month)
                
                if (starList.Load(fileName, NULL, NULL, mcsTRUE) == mcsFAILURE)
                {
                    // Ignore error (for test only)
                    errCloseStack();

                    // clear anyway:
                    starList.Clear();
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

                    // check Simbad Main ID
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
            logInfo("Performing GetStar scenario for '%s' ...", mainId);

            // Set star
            vobsSTAR star;
            star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  request.GetObjectRa(),  vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request.GetObjectDec(), vobsCATALOG_SIMBAD_ID);

            star.SetPropertyValue(vobsSTAR_POS_EQ_PMRA,  request.GetPmRa(),  vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_POS_EQ_PMDEC, request.GetPmDec(), vobsCATALOG_SIMBAD_ID);

            // Define SIMBAD SP_TYPE, OBJ_TYPES and main identifier (easier crossmatch):
            star.SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, spType, vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_OBJ_TYPES, objTypes, vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_ID_SIMBAD, mainId, vobsCATALOG_SIMBAD_ID);

            starList.AddAtTail(star);

            // init the scenario
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Init(&_scenarioSingleStar, &request, &starList), cmdName);

            FAIL_TIMLOG_CANCEL(_virtualObservatory.Search(&_scenarioSingleStar, starList), cmdName);

            // Save the current scenario search results:
            if (_useVOStarListBackup)
            {
                if (strlen(fileName) != 0)
                {
                    logInfo("Saving current VO StarList: %s", fileName);

                    if (starList.Save(fileName, mcsTRUE) == mcsFAILURE)
                    {
                        // Ignore error (for test only)
                        errCloseStack();
                    }
                }
            }
        }

        // If the star has not been found in catalogs (single star)
        nStars = starList.Size();
        if (nStars == 0)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, mainId, "CDS catalogs");
            }
            else
            {
                logInfo("Star '%.80s' has not been found in CDS catalogs", mainId);
            }
        }
        else if (nStars > 1) {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, mainId, "CDS catalogs");
            }
            else
            {
                logInfo("Star '%.80s' has too many results in CDS catalogs", mainId);
            }
        }
        else
        {
            // Get first star of the list:
            vobsSTAR* starPtr = starList.GetNextStar(mcsTRUE);

            // Set queried identifier in the Target_ID column:
            starPtr->GetTargetIdProperty()->SetValue(mainId, vobsORIG_COMPUTED);
                        
            // Update SIMBAD SP_TYPE, OBJ_TYPES and main identifier (easier crossmatch):
            starPtr->SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, spType, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
            starPtr->SetPropertyValue(vobsSTAR_OBJ_TYPES, objTypes, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE);
            starPtr->SetPropertyValue(vobsSTAR_ID_SIMBAD, mainId, vobsCATALOG_SIMBAD_ID, vobsCONFIDENCE_HIGH, mcsTRUE);

            // overwrite all fields given by GetStar parameters used by diameter estimation
            // VJHK + errors + SPTYPE and allow initial value correction in web form

            if (nbObjects == 1)
            {
                if (!isnan(uV)) {
                    // Fix missing errors:
                    if (isnan(ue_V)) {
                        ue_V = 0.1;
                    }
                    starPtr->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_V, uV, ue_V, vobsORIG_NONE, vobsCONFIDENCE_HIGH, mcsTRUE);
                }
            }
            else
            {
                // Fix missing V mag with SIMBAD information:
                if (!starPtr->IsPropertySet(vobsSTAR_PHOT_JHN_V) && !isnan(magV)) {
                    // Fix missing error as its origin(Simbad) != TYCHO2:
                    if (isnan(eMagV) || (eMagV < 0.1)) {
                        eMagV = 0.1;
                    }
                    starPtr->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_V, magV, eMagV, vobsCATALOG_SIMBAD_ID);
                }
            }

            if (nbObjects == 1)
            {
                if (!isnan(uJ)) {
                    // Fix missing error:
                    if (isnan(ue_J)) {
                        ue_J = 0.1;
                    }
                    starPtr->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_J, uJ, ue_J, vobsORIG_NONE, vobsCONFIDENCE_HIGH, mcsTRUE);
                }                
                if (!isnan(uH)) {
                    // Fix missing error:
                    if (isnan(ue_H)) {
                        ue_H = 0.1;
                    }
                    starPtr->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_H, uH, ue_H, vobsORIG_NONE, vobsCONFIDENCE_HIGH, mcsTRUE);
                }                
                // Fix missing error:

                if (!isnan(uK)) {
                    // Fix missing error:
                    if (isnan(ue_K)) {
                        ue_K = 0.1;
                    }
                    starPtr->SetPropertyValueAndError(vobsSTAR_PHOT_JHN_K, uK, ue_K, vobsORIG_NONE, vobsCONFIDENCE_HIGH, mcsTRUE);
                }                

                if (!IS_STR_EMPTY(uSpType)) {
                    starPtr->SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, uSpType, vobsORIG_NONE, vobsCONFIDENCE_HIGH, mcsTRUE);
                }
                
                // Update GetStarCmd with used values:
                if (starPtr->IsPropertySet(vobsSTAR_PHOT_JHN_V)) {
                    mcsDOUBLE val, err;
                    mcsSTRING32 num;
                    starPtr->GetPropertyValueAndError(vobsSTAR_PHOT_JHN_V, &val, &err);
                    
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("V", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", val);
                    p->SetUserValue(num);
                    
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("e_V", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", err);
                    p->SetUserValue(num);
                }
                if (starPtr->IsPropertySet(vobsSTAR_PHOT_JHN_J)) {
                    mcsDOUBLE val, err;
                    mcsSTRING32 num;
                    starPtr->GetPropertyValueAndError(vobsSTAR_PHOT_JHN_J, &val, &err);
                    
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("J", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", val);
                    p->SetUserValue(num);
                    
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("e_J", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", err);
                    p->SetUserValue(num);
                }
                if (starPtr->IsPropertySet(vobsSTAR_PHOT_JHN_H)) {
                    mcsDOUBLE val, err;
                    mcsSTRING32 num;
                    starPtr->GetPropertyValueAndError(vobsSTAR_PHOT_JHN_H, &val, &err);
                    
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("H", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", val);
                    p->SetUserValue(num);
                    
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("e_H", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", err);
                    p->SetUserValue(num);
                }
                if (starPtr->IsPropertySet(vobsSTAR_PHOT_JHN_K)) {
                    mcsDOUBLE val, err;
                    mcsSTRING32 num;
                    starPtr->GetPropertyValueAndError(vobsSTAR_PHOT_JHN_K, &val, &err);
                    
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("K", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", val);
                    p->SetUserValue(num);
                    
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("e_K", &p), cmdName);
                    snprintf(num, sizeof(num), "%.3lf", err);
                    p->SetUserValue(num);
                }
                if (starPtr->IsPropertySet(vobsSTAR_SPECT_TYPE_MK)) {
                    cmdPARAM* p;
                    FAIL_TIMLOG_CANCEL(getStarCmd.GetParam("SP_TYPE", &p), cmdName);
                    p->SetUserValue(starPtr->GetPropertyValue(vobsSTAR_SPECT_TYPE_MK)); 
                }
            }

            // Add a new calibrator in the list of calibrator (final output)
            calibratorList.AddAtTail(*starPtr);
        }

    } /* iterate on objectIds */


    // If stars have been found in catalogs
    if (calibratorList.Size() == 0)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, objectName, "SIMBAD");
        TIMLOG_CANCEL(cmdName);
    }
    else
    {
        // Prepare request to perform computations
        sclsvrREQUEST request;

        /* set diagnose flag */
        request.SetDiagnose(diagnoseFlag);

        /* use all object names */
        FAIL_TIMLOG_CANCEL(request.SetObjectName(objectName), cmdName);
        // Do not set Object Ra/Dec to skip distance computation (and sort)
        // Affect the file name
        if (IS_NOT_NULL(file))
        {
            FAIL_TIMLOG_CANCEL(request.SetFileName(file), cmdName);
        }
        FAIL_TIMLOG_CANCEL(request.SetSearchBand("K"), cmdName);
        
        // Optional parameters for ComputeVisibility()
        FAIL_TIMLOG_CANCEL(request.SetObservingWlen(wlen), cmdName);
        FAIL_TIMLOG_CANCEL(request.SetMaxBaselineLength(baseMax), cmdName);
        

        // Complete the calibrators list
        FAIL_TIMLOG_CANCEL(calibratorList.Complete(request), cmdName);

        // Pack the list result in a buffer in order to send it
        string xmlOutput;
        xmlOutput.reserve(2048);

        // use getStarCmd directly as GetCalCmd <> GetStarCmd:
        getStarCmd.AppendParamsToVOTable(xmlOutput);

        const char* command  = "GetStar";
        const char* header = "GetStar software (In case of problem, please report to jmmc-user-support@jmmc.fr)";

        // Disable trimming constant columns (replaced by parameter):
        // TODO: define a new request parameter
        const mcsLOGICAL trimColumns = mcsFALSE;

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
                                                 requestString, xmlOutput.c_str(), trimColumns, tlsLog), cmdName);
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
                                                  dynBuf, trimColumns, tlsLog), cmdName);
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
