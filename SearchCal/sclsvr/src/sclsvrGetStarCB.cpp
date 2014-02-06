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


    /*
     * TODO: use a request argument to enable/disable embedded logs
     */


    /* Enable log thread context if not in regression test mode (-noFileLine) */
    if (isTrue(logGetPrintFileLine()))
    {
        logEnableThreadContext();
    }

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
    if (isNotNull(msg))
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

    // Get star name
    char* objectName;
    if (getStarCmd.GetObjectName(&objectName) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get filename
    char* file = NULL;
    if (isTrue(getStarCmd.IsDefinedFile()) && getStarCmd.GetFile(&file) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get observed wavelength
    mcsDOUBLE wlen;
    if (getStarCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get baseline
    mcsDOUBLE baseline;
    if (getStarCmd.GetWlen(&baseline) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Get star position from SIMBAD
    mcsSTRING32 ra, dec, spType;
    mcsDOUBLE pmRa, pmDec;

    if (simcliGetCoordinates(objectName, ra, dec, &pmRa, &pmDec, spType) == mcsFAILURE)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, objectName, "SIMBAD");

        TIMLOG_CANCEL(cmdName)
    }

    logInfo("GetStar[%s]: RA/DEC='%s %s' pmRA/pmDEC=%.1lf %.1lf spType='%s'", objectName, ra, dec, pmRa, pmDec, spType);

    // Prepare request to search information in other catalog
    sclsvrREQUEST request;
    if (request.SetObjectName(objectName) == mcsFAILURE)
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
    if (isNotNull(file) && (request.SetFileName(file) == mcsFAILURE))
    {
        TIMLOG_CANCEL(cmdName)
    }

    if (request.SetSearchBand("K") == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    if (request.SetObservingWlen(wlen) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    if (request.SetMaxBaselineLength(baseline) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }


    // flag to load/save vobsStarList results:
    bool _useVOStarListBackup = vobsIsDevFlag();


    vobsSTAR_LIST starList("GetStar");

    mcsSTRING512 fileName;

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
        // Set star
        vobsSTAR star;
        star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  request.GetObjectRa(),  vobsNO_CATALOG_ID);
        star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request.GetObjectDec(), vobsNO_CATALOG_ID);

        star.SetPropertyValue(vobsSTAR_POS_EQ_PMRA,  request.GetPmRa(),  vobsNO_CATALOG_ID);
        star.SetPropertyValue(vobsSTAR_POS_EQ_PMDEC, request.GetPmDec(), vobsNO_CATALOG_ID);

        // Optional SIMBAD SP_TYPE_MK:
        logTest("%s", spType);

        star.SetPropertyValue(vobsSTAR_SPECT_TYPE_MK, spType, vobsNO_CATALOG_ID);

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

    if (isNotNull(dynBuf))
    {
        dynBuf->Reset();
    }

    // If the star has been found in catalog
    if (starList.Size() == 0)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, objectName, "CDS catalogs");
    }
    else
    {
        // Get first star of the list
        sclsvrCALIBRATOR calibrator(*starList.GetNextStar(mcsTRUE));

        // Prepare information buffer:
        miscoDYN_BUF infoMsg;
        infoMsg.Reserve(1024);

        // Complete missing properties of the calibrator
        if (calibrator.Complete(request, infoMsg) == mcsFAILURE)
        {
            // Ignore error
            errCloseStack();
        }

        // Build the list of calibrator (final output)
        sclsvrCALIBRATOR_LIST calibratorList("Calibrators");
        calibratorList.AddAtTail(calibrator);

        // Pack the list result in a buffer in order to send it
        if (calibratorList.Size() != 0)
        {
            string xmlOutput;
            xmlOutput.reserve(2048);

            // use getStarCmd directly as GetCalCmd <> GetStarCmd:
            //            request.AppendParamsToVOTable(xmlOutput);
            getStarCmd.AppendParamsToVOTable(xmlOutput);

            const char* voHeader = "GetStar software (In case of problem, please report to jmmc-user-support@ujf-grenoble.fr)";

            const mcsLOGICAL trimColumns = mcsFALSE; // TODO: define a new request parameter

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
                if (strcmp(miscGetExtension(fileName), "vot") == 0)
                {
                    // Save the list as a VOTable v1.1
                    if (calibratorList.SaveToVOTable(request.GetFileName(), voHeader, softwareVersion,
                                                     requestString, xmlOutput.c_str(), trimColumns, tlsLog) == mcsFAILURE)
                    {
                        TIMLOG_CANCEL(cmdName)
                    }
                }
                else
                {
                    if (calibratorList.Save(request.GetFileName(), request) == mcsFAILURE)
                    {
                        TIMLOG_CANCEL(cmdName)
                    }
                }
            }

            // Give back CDATA for msgMESSAGE reply.
            if (isNotNull(dynBuf))
            {
                if (isNotNull(msg))
                {
                    calibratorList.Pack(dynBuf);
                }
                else
                {
                    // Otherwise give back a VOTable (DO NOT trim columns)
                    if (calibratorList.GetVOTable(voHeader, softwareVersion, requestString, xmlOutput.c_str(), dynBuf, trimColumns, tlsLog) == mcsFAILURE)
                    {
                        TIMLOG_CANCEL(cmdName)
                    }
                }
            }
        }
    }

    if (isNotNull(msg))
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
