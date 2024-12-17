/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * sclsvrGetCalCB class definition.
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
#include "thrd.h"
#include "sdb.h"


/*
 * SCALIB Headers
 */
#include "alx.h"
#include "vobs.h"


/*
 * Local Headers
 */
#include "sclsvrVersion.h"
#include "sclsvrErrors.h"
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrSCENARIO_JSDC.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"

/** allow GetCal query cache (dev only) */
#define ALLOW_CACHE     0

/*
 * Local Macros
 */

/** Initialize static members */
bool sclsvrSERVER::sclsvrSERVER_buildJSDC = false;
bool sclsvrSERVER::sclsvrSERVER_queryJSDC = false;
bool sclsvrSERVER::sclsvrSERVER_queryJSDC_Faint = false;

/*
 * Public methods
 */

/**
 * Return the current index of the catalog being queried.
 *
 * @param buffer will an already allocated buffer to contain the catalog name.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSERVER::GetStatus(mcsSTRING256* buffer, mcsINT32 timeoutInSec)
{
    // Wait for an updated status
    FAIL(_status.Read(buffer, mcsTRUE, timeoutInSec));

    return mcsSUCCESS;
}

/**
 * Callback method for GETCAL command.
 *
 * It handles the request contained in received message, processes it and
 * returns the list of found calibrators.
 *
 * @param msg message containing request
 *
 * \return evhCB_NO_DELETE.
 */
evhCB_COMPL_STAT sclsvrSERVER::GetCalCB(msgMESSAGE &msg, void*)
{
    // Get calibrators
    miscoDYN_BUF dynBuf;
    mcsCOMPL_STAT complStatus = ProcessGetCalCmd(msg.GetBody(), &dynBuf, &msg);

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

    // Set reply
    miscDynSIZE nbStoredBytes;
    dynBuf.GetNbStoredBytes(&nbStoredBytes);

    if (nbStoredBytes != 0)
    {
        msg.SetBody(dynBuf.GetBuffer());
    }
    else
    {
        msg.SetBody("");
    }

    // Send reply
    if (SendReply(msg) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    return evhCB_NO_DELETE;
}

/**
 * Handle GETCAL command.
 *
 * It handles the given query corresponding to the parameter list of GETCAL
 * command, processes it and returns the list of found calibrators.
 *
 * @param msg message containing request
 * @param dynBuf dynamical buffer where calibrator list will be stored
 *
 * \return evhCB_NO_DELETE.
 */
mcsCOMPL_STAT sclsvrSERVER::GetCal(const char* query, miscoDYN_BUF* dynBuf)
{
    // Check reentrance:
    if (_working)
    {
        logWarning("GetCal reentrance detected.");
        return mcsFAILURE;
    }

    // Update working flag
    _working = true;

    // Get calibrators
    mcsCOMPL_STAT complStatus = ProcessGetCalCmd(query, dynBuf, NULL);

    // Update status to inform request processing is completed
    FAIL(_status.Write("0"));

    return complStatus;
}

/**
 * GETCAL command processing method.
 *
 * This method is called by GETCAL command callback. It selects appropriated
 * scenario, executes it and returns resulting list of calibrators
 *
 * @param query user query containing all command parameters in string format
 * @param dynBuf dynamical buffer where calibrator list will be stored
 * @param msg message corresponding to the received command. If not NULL, a
 * thread is started and intermediate replies are sent giving the request
 * processing status.
 *
 * @return Upon successful completion returns mcsSUCCESS. Otherwise,
 * mcsFAILURE is returned.
 */
mcsCOMPL_STAT sclsvrSERVER::ProcessGetCalCmd(const char* query,
                                             miscoDYN_BUF* dynBuf,
                                             msgMESSAGE* msg)
{
    static const char* cmdName = "GETCAL";

    // Build the request object from the parameters of the command
    sclsvrREQUEST request;
    FAIL(request.Parse(query));

    // Get the request as a string for the case of Save in VOTable
    mcsSTRING1024 requestString;
    strncpy(requestString, query, sizeof (requestString) - 1);

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

        // Launch the status monitoring thread
        FAIL_TIMLOG_CANCEL(thrdThreadCreate(&monitorTask), cmdName);
    }

    bool isRegressionTest   = IS_FALSE(logGetPrintFileLine());
    bool diagnose           = IS_TRUE(request.IsDiagnose());

    if (diagnose)
    {
        logInfo("diagnose mode enabled.");
    }

    bool doFilterDiameterOK = !diagnose && !isRegressionTest;
    bool doUseThreadLog     = (diagnose || alxIsDevFlag());

    // Enable trimming constant columns (replaced by parameter) EXCEPT JSDC:
    // TODO: define a new request parameter
    mcsLOGICAL trimColumns = mcsTRUE;

    // If the request should return bright stars
    vobsSCENARIO* scenario;
    if (IS_TRUE(request.IsBright()))
    {
        if (request.GetSearchAreaGeometry() == vobsBOX)
        {
            // According to the desired band
            const char* band = request.GetSearchBand();
            switch (band[0])
            {
                case 'I':
                case 'J':
                case 'H':
                case 'K':
                    // Use the JSDC Catalog Query Scenario or the Bright K Scenario
                    if (sclsvrSERVER_queryJSDC)
                    {
                        scenario = &_scenarioJSDC_Query;
                    }
                    else
                    {
                        scenario = &_scenarioBrightK;
                    }
                    break;

                case 'V':
                    // Use the JSDC Catalog Query Scenario or the Bright V Scenario
                    if (sclsvrSERVER_queryJSDC)
                    {
                        scenario = &_scenarioJSDC_Query;
                    }
                    else
                    {
                        scenario = &_scenarioBrightV;
                    }
                    break;

                case 'L':
                case 'M':
                case 'N':
                    if (sclsvrSERVER_queryJSDC)
                    {
                        // Use the JSDC Catalog Query Scenario
                        scenario = &_scenarioJSDC_Query;
                    }
                    else
                    {
                        errAdd(sclsvrERR_UNKNOWN_BRIGHT_BAND, band);
                        TIMLOG_CANCEL(cmdName)
                    }
                    break;

                case '0':
                    if (sclsvrSERVER_buildJSDC)
                    {
                        // Use the JSDC Catalog Scenario
                        scenario = &_scenarioJSDC;

                        // Reuse scenario results for JSDC:
                        _useVOStarListBackup = true;

                        // Disable diamFlag filter:
                        doFilterDiameterOK = false;

                        // Disable Thread log:
                        doUseThreadLog = false;

                        // Disable trimming constant columns:
                        trimColumns = mcsFALSE;

                        // Define correctly the band to K:
                        request.SetSearchBand("K");

                        // Set JSDC mode:
                        request.SetJSDCMode(mcsTRUE);
                        break;
                    }

                default:
                    errAdd(sclsvrERR_UNKNOWN_BRIGHT_BAND, band);
                    TIMLOG_CANCEL(cmdName)
            }
        }
        else
        {
            if (request.GetSearchAreaGeometry() == vobsCIRCLE)
            {
                errAdd(sclsvrERR_INVALID_SEARCH_AREA, "bright", "rectangular");
            }

            TIMLOG_CANCEL(cmdName)
        }
    }
    else
    {
        // If radius has not been given, set it to 0; i.e. determine by SW
        if (request.GetSearchAreaGeometry() != vobsCIRCLE)
        {
            request.SetSearchArea(0.0);
        }

        // Faint is only avalable in K band
        const char* band = request.GetSearchBand();
        switch (band[0])
        {
            case 'J':
            case 'H':
            case 'K':
                // Use the JSDC Catalog Query Scenario or the Faint K Scenario
                if (IsQueryJSDCFaint())
                {
                    scenario = &_scenarioJSDC_Query;
                }
                else
                {
                    scenario = &_scenarioFaintK;
                }
                break;

            case 'I':
            case 'V':
            case 'L':
            case 'M':
            case 'N':
                if (IsQueryJSDCFaint())
                {
                    // Use the JSDC Catalog Query Scenario
                    scenario = &_scenarioJSDC_Query;
                }
                else
                {
                    errAdd(sclsvrERR_UNKNOWN_FAINT_BAND, band);
                    TIMLOG_CANCEL(cmdName)
                }
                break;

            case '0':
                if (sclsvrSERVER_buildJSDC)
                {
                    // Use the JSDC Faint Catalog Scenario
                    scenario = &_scenarioJSDC_Faint;

                    // Reuse scenario results for JSDC:
                    _useVOStarListBackup = true;

                    // Disable diamFlag filter:
                    doFilterDiameterOK = false;

                    // Disable Thread log:
                    doUseThreadLog = false;

                    // Disable trimming constant columns:
                    trimColumns = mcsFALSE;

                    // Define correctly the band to K:
                    request.SetSearchBand("K");

                    // Force Bright flag for missing magnitude computation:
                    request.SetBrightFlag(mcsTRUE);

                    // Set JSDC mode:
                    request.SetJSDCMode(mcsTRUE);
                    break;
                }

            default:
                errAdd(sclsvrERR_UNKNOWN_FAINT_BAND, band);
                TIMLOG_CANCEL(cmdName)
        }
    }

    /*
     * if DEV_FLAG: skip CDS queries ie always try to reuse previous search results
     *        else: always perform CDS queries
     */
    if (ALLOW_CACHE && alxIsDevFlag())
    {
        _useVOStarListBackup = true;
    }

    if (doUseThreadLog && !isRegressionTest)
    {
        /* Enable log thread context if not in regression test mode (-noFileLine) */
        logEnableThreadContext();
    }

    /* Define the cancellation flag in thread local storage */
    vobsSetCancelFlag(_cancelFlag);


    // Build the list of calibrator (final output)
    sclsvrCALIBRATOR_LIST calibratorList("Calibrators");

    {
        // encapsulate the star list in one block to destroy it asap
        mcsSTRING512 fileName;

        // Build the list of star which will come from the virtual observatory
        vobsSTAR_LIST starList("GetCal");

        // Load previous scenario search results:
        if (_useVOStarListBackup)
        {
            // Define & resolve the file name once:
            strcpy(fileName, "$MCSDATA/tmp/GetCal/SearchListBackup_");
            strcat(fileName, scenario->GetScenarioName());
            strcat(fileName, ".dat");

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

        if (starList.IsEmpty())
        {
            // Initialize the scenario
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Init(scenario, &request), cmdName);

            // Start the research in the virtual observatory
            FAIL_TIMLOG_CANCEL(_virtualObservatory.Search(scenario, starList), cmdName);

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

        if (starList.IsFreeStarPointers())
        {
            // Fill the calibrator list from the returned star list
            // Note: using Move() reduces memory footprint
            // as stars are converted to calibrators and deleted from memory
            // and the starList is empty after the Move operation completes !
            calibratorList.Move(starList);
        }
        else
        {
            // shared star pointers (JSDC data)
            calibratorList.Copy(starList);
        }
    }

    // Check cancellation:
    if (IsCancelled())
    {
        TIMLOG_CANCEL(cmdName)
    }


    // Complete the calibrators list
    FAIL_TIMLOG_CANCEL(calibratorList.Complete(request), cmdName);

    // Check cancellation:
    if (IsCancelled())
    {
        TIMLOG_CANCEL(cmdName)
    }


    // Filter calibrators with diamFlag not OK (production mode):
    if (doFilterDiameterOK && !alxIsDevFlag())
    {
        FAIL_TIMLOG_CANCEL(calibratorList.FilterDiameterOk(), cmdName);
    }

    // Check cancellation:
    if (IsCancelled())
    {
        TIMLOG_CANCEL(cmdName)
    }


    // If requested, remove the science object if it belongs to the calibrator list:
    if (IS_TRUE(request.IsNoScienceStar()))
    {
        // 1) Make a copy (star pointers) of the calibrator list in order to create a temp list
        // containing all calibrators within 1 arcsec in RA and DEC of the
        // science object coordinates
        vobsSTAR_LIST scienceObjects("ScienceObjects");

        // note: calibrator list manages star pointers (i.e. freeStarPointers = true)
        scienceObjects.CopyRefs(calibratorList, mcsFALSE);

        // 2) Create a filter to only get stars within 1 arcsecond of the original science object
        vobsDISTANCE_FILTER distanceFilter("ScienceStar");
        distanceFilter.SetDistanceValue(request.GetObjectRa(), request.GetObjectDec(), alxARCSEC_IN_DEGREES);

        // 3) Apply the filter to the copied calibrator list
        distanceFilter.Apply(&scienceObjects);

        // 4) Remove from the original calibrator list any star left by the
        // filter in the temporary list
        vobsSTAR* starPtr = scienceObjects.GetNextStar(mcsTRUE);
        while (IS_NOT_NULL(starPtr))
        {
            // Get Star ID
            mcsSTRING64 starId;
            FAIL_TIMLOG_CANCEL(starPtr->GetId(starId, sizeof (starId)), cmdName);
            logTest("(What should be) Science star '%s' has been removed.", starId);

            // note: starPtr will be freed by calibratorList and is still present
            // but invalid in scienceObjects
            // Note: removeRef operation can be used as star pointers are coming from the calibrator list !
            calibratorList.RemoveRef(starPtr);
            starPtr = scienceObjects.GetNextStar();
        }
    }


    // Check cancellation:
    if (IsCancelled())
    {
        TIMLOG_CANCEL(cmdName)
    }

    if (IS_NOT_NULL(dynBuf))
    {
        dynBuf->Reset();
    }

    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    {
        string xmlOutput;
        xmlOutput.reserve(2048);
        request.AppendParamsToVOTable(xmlOutput);

        const char* command  = "SearchCal";
        const char* voHeader = "SearchCal software: http://www.jmmc.fr/searchcal (In case of problem, please report to jmmc-user-support@jmmc.fr)";

        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof (softwareVersion) - 1, "%s v%s", "SearchCal Server", sclsvrVERSION);

        // Get the thread log:
        const char* tlsLog = logContextGetBuffer();

        // If a filename has been given, store results as file
        if (strlen(request.GetFileName()) != 0)
        {
            mcsSTRING32 fileName;
            strcpy(fileName, request.GetFileName());

            // If the extension is .vot, save as VO table
            if (strcmp(miscGetExtension(fileName), "vot") == 0)
            {
                // Save the list as a VOTable v1.1  (trim columns)
                FAIL_TIMLOG_CANCEL(calibratorList.SaveToVOTable(command, request.GetFileName(), voHeader, softwareVersion,
                                                 requestString, xmlOutput.c_str(), trimColumns, tlsLog), cmdName);
            }
            else
            {
                FAIL_TIMLOG_CANCEL(calibratorList.Save(request.GetFileName(), request), cmdName);
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
                // Otherwise give back a VOTable (trim columns)
                FAIL_TIMLOG_CANCEL(calibratorList.GetVOTable(command, voHeader, softwareVersion, requestString, xmlOutput.c_str(),
                                              dynBuf, trimColumns, tlsLog), cmdName);
            }
        }
    }

    // Monitoring task is started only when msgMESSAGE is received.
    if (IS_NOT_NULL(msg))
    {
        // Wait for the monitoring task end
        FAIL_TIMLOG_CANCEL(thrdThreadWait(&monitorTask), cmdName);
    }

    // Stop timer log
    timlogStop(cmdName);

    return mcsSUCCESS;
}

/*___oOo___*/
