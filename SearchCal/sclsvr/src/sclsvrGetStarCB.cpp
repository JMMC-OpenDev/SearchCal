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
#define MAX_OBJECT_IDS 2000

/*
 * Local Macros
 */

/* Discard time counter */
#define TIMLOG_CANCEL(cmdName) { \
    timlogCancel(cmdName);       \
    return evhCB_NO_DELETE | evhCB_FAILURE; \
}

/*
 * Public methods
 */
evhCB_COMPL_STAT sclsvrSERVER::GetStarCB(msgMESSAGE &msg, void*)
{
    miscoDYN_BUF dynBuf;
    evhCB_COMPL_STAT complStatus = ProcessGetStarCmd(msg.GetBody(), &dynBuf, &msg);

    return complStatus;
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
    evhCB_COMPL_STAT complStatus = ProcessGetStarCmd(query, dynBuf, NULL);

    // Update status to inform request processing is completed
    FAIL(_status.Write("0"));

    FAIL_COND(complStatus != evhCB_SUCCESS);

    return mcsSUCCESS;
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
evhCB_COMPL_STAT sclsvrSERVER::ProcessGetStarCmd(const char* query,
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
    if (getStarCmd.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

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
        if (thrdThreadCreate(&monitorTask) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
    }

    if (IS_NOT_NULL(dynBuf))
    {
        dynBuf->Reset();
    }

    // Get filename
    char* file = NULL;
    if (IS_TRUE(getStarCmd.IsDefinedFile()) && getStarCmd.GetFile(&file) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get observed wavelength
    mcsDOUBLE wlen;
    if (getStarCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get baseMax
    mcsDOUBLE baseMax;
    if (getStarCmd.GetBaseMax(&baseMax) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get star name
    char* objectName;
    if (getStarCmd.GetObjectName(&objectName) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get format parameter:
    char* outputFormat;
    FAIL(getStarCmd.GetFormat(&outputFormat));

    // Get diagnose flag:
    mcsLOGICAL diagnoseFlag = mcsFALSE;
    FAIL(getStarCmd.GetDiagnose(&diagnoseFlag));


    // Parse objectName to get multiple star identifiers (separated by comma)
    mcsUINT32    nbObjects = 0;
    mcsSTRING256 objectIds[MAX_OBJECT_IDS];

    logInfo("objectNames: '%s'", objectName);

    /* may fail*/
    if (miscSplitString(objectName, ',', objectIds, MAX_OBJECT_IDS, &nbObjects) == mcsFAILURE)
    {
        /* too many identifiers */
        TIMLOG_CANCEL(cmdName)
    }
    logDebug("nbObjects: %d", nbObjects);


    const bool isRegressionTest = IS_FALSE(logGetPrintFileLine());
    /* if multiple objects, disable log */
    const bool diagnose = (nbObjects <= 1) && (IS_TRUE(diagnoseFlag) || vobsIsDevFlag());

    if (diagnose)
    {
        logInfo("diagnose mode enabled.");
    }

    /* Enable log thread context if not in regression test mode (-noFileLine) */
    if (diagnose && !isRegressionTest)
    {
        logEnableThreadContext();
    }


    // Reuse scenario results for GetStar:
    _useVOStarListBackup = vobsIsDevFlag();
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
        mcsSTRING64 spType, objTypes, mainId;
        mcsDOUBLE pmRa, pmDec;

        if (simcliGetCoordinates(objectId, ra, dec, &pmRa, &pmDec, spType, objTypes, mainId) == mcsFAILURE)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, objectId, "SIMBAD");

                TIMLOG_CANCEL(cmdName)
            }
            else
            {
                logInfo("Star named '%.80s' has not been found in SIMBAD", objectId);
                continue;
            }
        }

        logInfo("GetStar[%s]: RA/DEC='%s %s' pmRA/pmDEC=%.1lf %.1lf spType='%s' objTypes='%s' IDS='%s'",
                objectId, ra, dec, pmRa, pmDec, spType, objTypes, mainId);

        // Prepare request to search information in other catalog
        sclsvrREQUEST request;
        if (request.SetObjectName(objectId) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetObjectRa(ra) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetObjectDec(dec) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetPmRa((mcsDOUBLE) pmRa) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetPmDec((mcsDOUBLE) pmDec) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        // Affect the file name
        if (IS_NOT_NULL(file) && (request.SetFileName(file) == mcsFAILURE))
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetSearchBand("K") == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }


        // clear anyway:
        starList.Clear();

        // Load previous scenario search results:
        if (_useVOStarListBackup)
        {
            // Define & resolve the file name once:
            strcpy(fileName, "$MCSDATA/tmp/GetStar/SearchListBackup_");
            strcat(fileName, _scenarioSingleStar.GetScenarioName());
            strcat(fileName, "_");
            strcat(fileName, request.GetObjectRa());
            strcat(fileName, "_");
            strcat(fileName, request.GetObjectDec());
            strcat(fileName, ".dat");

            FAIL(miscReplaceChrByChr(fileName, ' ', '_'));

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

                if (starList.Load(fileName, NULL, NULL, mcsTRUE) == mcsFAILURE)
                {
                    // Ignore error (for test only)
                    errCloseStack();

                    // clear anyway:
                    starList.Clear();
                }
            }
        }

        // Try searching in JSDC:
        if (starList.IsEmpty() && sclsvrSERVER_queryJSDC_Faint)
        {
            // Use the JSDC Catalog Query Scenario (faint)
            request.SetBrightFlag(mcsFALSE);

            // 2 arcsec to match Star(s) (identifier check):
            mcsDOUBLE filterRadius = (mcsDOUBLE) (2.0 * alxARCSEC_IN_DEGREES);

            request.SetSearchArea(filterRadius * alxDEG_IN_ARCMIN);

            // init the scenario
            if (_virtualObservatory.Init(&_scenarioJSDC_Query, &request, &starList) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }

            if (_virtualObservatory.Search(&_scenarioJSDC_Query, starList) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }

            mcsUINT32 nStars = starList.Size();

            if (nStars > 1)
            {
                logInfo("GetStar: too many results (%d) from JSDC; perform GetStar scenario", nStars);
            }
            else if (nStars == 1)
            {
                // check ID SIMBAD ?
                vobsSTAR* star = starList.GetNextStar(mcsTRUE);

                if (IS_NOT_NULL(star))
                {
                    vobsSTAR_PROPERTY* property = star->GetProperty(vobsSTAR_ID_SIMBAD);
                    if (isPropSet(property))
                    {
                        mcsSTRING64 jsdcId, simbadId;
                        strncpy(jsdcId, property->GetValue(), sizeof (jsdcId) - 1);
                        strncpy(simbadId, mainId, sizeof (simbadId) - 1);
                        // remove space characters
                        miscDeleteChr((char *) jsdcId, ' ', mcsTRUE);
                        miscDeleteChr((char *) simbadId, ' ', mcsTRUE);

                        logTest("Found star [%s] for SIMBAD ID [%s]", jsdcId, simbadId);

                        // check Simbad Main ID
                        if (strcmp(jsdcId, simbadId) != 0)
                        {
                            logWarning("Mismatch identifiers: [%s] vs [%s]", jsdcId, simbadId);
                            starList.Clear();
                        }
                    }
                }
            }
        }

        if (starList.IsEmpty())
        {
            // Set star
            vobsSTAR star;
            star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  request.GetObjectRa(),  vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request.GetObjectDec(), vobsCATALOG_SIMBAD_ID);

            star.SetPropertyValue(vobsSTAR_POS_EQ_PMRA,  request.GetPmRa(),  vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_POS_EQ_PMDEC, request.GetPmDec(), vobsCATALOG_SIMBAD_ID);

            // Define SIMBAD SP_TYPE, OBJ_TYPES and queried identifier (easier crossmatch):
            star.SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, spType, vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_OBJ_TYPES, objTypes, vobsCATALOG_SIMBAD_ID);
            star.SetPropertyValue(vobsSTAR_ID_SIMBAD, mainId, vobsCATALOG_SIMBAD_ID);

            starList.AddAtTail(star);

            // init the scenario
            if (_virtualObservatory.Init(&_scenarioSingleStar, &request, &starList) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }

            if (_virtualObservatory.Search(&_scenarioSingleStar, starList) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }

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
        if (starList.Size() == 0)
        {
            if (nbObjects == 1)
            {
                errAdd(sclsvrERR_STAR_NOT_FOUND, objectId, "CDS catalogs");
            }
            else
            {
                logInfo("Star named '%.80s' has not been found in CDS catalogs", objectId);
            }
        }
        else
        {
            // Get first star of the list and add a new calibrator
            // in the list of calibrator (final output)
            calibratorList.AddAtTail(*starList.GetNextStar(mcsTRUE));
        }
    } /* iterate on objectIds */


    // If stars have been found in catalogs
    if (calibratorList.Size() != 0)
    {
        // Prepare request to perform computations
        sclsvrREQUEST request;

        /* set diagnose flag */
        request.SetDiagnose(diagnoseFlag);

        /* use all object names */
        if (request.SetObjectName(objectName) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        // Do not set Object Ra/Dec to skip distance computation (and sort)
        // Affect the file name
        if (IS_NOT_NULL(file) && (request.SetFileName(file) == mcsFAILURE))
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetSearchBand("K") == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        // Optional parameters for ComputeVisibility()
        if (request.SetObservingWlen(wlen) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        if (request.SetMaxBaselineLength(baseMax) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }


        // Complete the calibrators list
        if (calibratorList.Complete(request) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }

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
                if (calibratorList.SaveToVOTable(command, request.GetFileName(), header, softwareVersion,
                                                 requestString, xmlOutput.c_str(), trimColumns, tlsLog) == mcsFAILURE)
                {
                    TIMLOG_CANCEL(cmdName)
                }
            }
            else
            {
                // Save the list as a TSV file
                if (calibratorList.SaveTSV(request.GetFileName(), header, softwareVersion, requestString) == mcsFAILURE)
                {
                    TIMLOG_CANCEL(cmdName)
                }
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
                    if (calibratorList.GetVOTable(command, header, softwareVersion, requestString, xmlOutput.c_str(),
                                                  dynBuf, trimColumns, tlsLog) == mcsFAILURE)
                    {
                        TIMLOG_CANCEL(cmdName)
                    }
                }
                else
                {
                    // Otherwise, give back a TSV file
                    if (calibratorList.GetTSV(header, softwareVersion, requestString, dynBuf) == mcsFAILURE)
                    {
                        TIMLOG_CANCEL(cmdName)
                    }
                }
            }
        }
    }

    if (IS_NOT_NULL(msg))
    {
        // Wait for the actionForwarder thread end
        if (thrdThreadWait(&monitorTask) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
    }

    // Stop timer log
    timlogStop(cmdName);

    return evhCB_SUCCESS;
}

/*___oOo___*/
