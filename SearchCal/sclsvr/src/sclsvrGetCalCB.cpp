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
#include "sclsvrSCENARIO_BRIGHT_N.h"

/*
 * Local Macros
 */

/* Discard time counter */
#define TIMLOG_CANCEL(cmdName) { \
    timlogCancel(cmdName);       \
    return mcsFAILURE;           \
}

/*
 * Public methods
 */
/**
 * Return the current index of the catalog being queried.
 *
 * @param buffer will an already allocated buffer to contain the catalog name.
 *
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned 
 */
mcsCOMPL_STAT sclsvrSERVER::GetStatus(char* buffer, mcsINT32 timeoutInSec)
{
    logTrace("sclsvrSERVER::GetStatus()");

    // Wait for an updated status
    if (_status.Read(buffer, mcsTRUE, timeoutInSec) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

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
    logTrace("sclsvrSERVER::GetCalCB()");

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
    mcsUINT32 nbStoredBytes;
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
    logTrace("sclsvrSERVER::GetCal()");

    // Get calibrators
    mcsCOMPL_STAT complStatus = ProcessGetCalCmd(query, dynBuf, NULL);

    // Update status to inform request processing is completed 
    if (_status.Write("0") == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
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
mcsCOMPL_STAT sclsvrSERVER::ProcessGetCalCmd(const char*   query, 
                                             miscoDYN_BUF* dynBuf, 
                                             msgMESSAGE*   msg)
{
    logTrace("sclsvrSERVER::ProcessGetCalCmd()");

    static const char* cmdName = "GETCAL";

    // Build the request object from the parameters of the command
    sclsvrREQUEST request;
    if (request.Parse(query) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Get the request as a string for the case of Save in VOTable
    mcsSTRING256 requestString;
    strcpy(requestString, query);
 
    // Start timer log
    timlogInfoStart(cmdName);
    
    // Monitoring task
    thrdTHREAD_STRUCT monitorTask;

    // If request comes from msgMESSAGE, start monitoring task send send
    // request progression status
    if (msg != NULL)
    {
        // Monitoring task parameters
        sclsvrMONITOR_TASK_PARAMS monitorTaskParams;
        monitorTaskParams.server  = this;
        monitorTaskParams.message = msg;
        monitorTaskParams.status  = &_status;
    
        // Monitoring task
        monitorTask.function  = sclsvrMonitorTask;
        monitorTask.parameter = (thrdFCT_ARG*)&monitorTaskParams;
        
        // Launch the status monitoring thread
        if (thrdThreadCreate(&monitorTask) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
    }

    // If the request should return bright starts
    vobsSCENARIO *scenario;
    if ((request.IsBright() == mcsTRUE) && (request.GetSearchAreaGeometry() == vobsBOX))
    {
        // According to the desired band
        const char* band = request.GetSearchBand();
        switch(band[0])
        {
            case 'I':
            case 'J':
            case 'H':
            case 'K':
                // Load Bright K Scenario
                scenario = &_scenarioBrightK;
                break;

            case 'V':
                // Load Bright V Scenario
                scenario = &_scenarioBrightV;
                break;

            case 'N':
                // Load Bright N Scenario
                scenario = &_scenarioBrightN;
                break;

            case '1':
                // Load Bright K Catalog Scenario
                scenario = &_scenarioBrightKCatalog;
                break;

            case '0':
                // Load JSDC Catalog Scenario
                scenario = &_scenarioJSDC;
		request.SetSearchBand("K");
                break;

            default:
                errAdd(sclsvrERR_UNKNOWN_BRIGHT_BAND, band);

                TIMLOG_CANCEL(cmdName)
        }
    }
    else if ((request.IsBright() == mcsFALSE))
    {
        // If radius has not been given, set it to 0; i.e. determine by SW
        if (request.GetSearchAreaGeometry() != vobsCIRCLE) 
        {
            request.SetSearchArea(0.0);
        }

        // Faint is only avalable in K band
        const char* band = request.GetSearchBand();
        switch(band[0])
        {
            case 'K':
                // Load Faint K Scenario
                scenario = &_scenarioFaintK;
                break;

            default:
                errAdd(sclsvrERR_UNKNOWN_FAINT_BAND, band);

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

    // Load the scenario
    if (scenario->Init(&request) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Build the list of calibrator (final output)
    sclsvrCALIBRATOR_LIST calibratorList;

    {
        // encapsulate the star list in one block to destroy it asap

        // Build the list of star which will come from the virtual observatory
        vobsSTAR_LIST starList;

        // Start the research in the virtual observatory
        if (_virtualObservatory.Search(scenario, request, starList) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }

        // Get the returned star list and create a calibrator list from it
        if (calibratorList.Copy(starList) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
    }

    // Complete the calibrators list
    if (calibratorList.Complete(request) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    
    // If requested, remove the science object if it belongs to the calibrator list:
    if (request.IsNoScienceStar() == mcsTRUE)
    {
        // 1) Make a copy (star pointers) of the calibrator list in order to create a temp list
        // containing all calibrators within 1 arcsec in RA and DEC of the
        // science object coordinates
        vobsSTAR_LIST scienceObjects;

        // note: calibrator list manages star pointers (i.e. freeStarPointers = true)
        scienceObjects.CopyRefs(calibratorList, mcsFALSE); 

        // 2) Create a filter to only get stars within 1 arcsecond of the original science object
        vobsDISTANCE_FILTER distanceFilter("");
        distanceFilter.SetDistanceValue(request.GetObjectRa(), request.GetObjectDec(), sclsvrARCSEC_IN_DEGREES);

        // 3) Apply the filter to the copied calibrator list
        distanceFilter.Apply(&scienceObjects);

        // 4) Remove from the original calibrator list any star left by the
        // filter in the temporary list
        vobsSTAR* currentStar = scienceObjects.GetNextStar(mcsTRUE);
        while (currentStar != NULL)
        {
            // Get Star ID
            mcsSTRING64 starId;
            if (currentStar->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }
            logTest("(What should be) Science star '%s' has been removed.", starId);
            
            // note: currentStar will be freed by calibratorList and is still present 
            // but invalid in scienceObjects
            calibratorList.Remove(*currentStar);
            currentStar = scienceObjects.GetNextStar();
        }
    }
    
    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    { 
        string xmlOutput;
        xmlOutput.reserve(2048);
        request.AppendParamsToVOTable(xmlOutput);
        
        const char* voHeader = "SearchCal software: http://www.jmmc.fr/searchcal (In case of problem, please report to jmmc-user-support@ujf-grenoble.fr)";
        
        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof(softwareVersion) - 1, "%s v%s", "SearchCal", sclsvrVERSION);

        // If a filename has been given, store results as file
        if (strcmp(request.GetFileName(), "") != 0)
        {
            mcsSTRING32 fileName;
            strcpy(fileName, request.GetFileName());
            
            // If the extension is .vot, save as VO table
            if (strcmp(miscGetExtension(fileName), "vot") == 0)
            {
                // Save the list as a VOTable v1.1
                if (calibratorList.SaveToVOTable(request.GetFileName(), voHeader, softwareVersion,
                                                 requestString, xmlOutput.c_str()) == mcsFAILURE)
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
        if (dynBuf != NULL)
        {
            if (msg != NULL)
            {
                calibratorList.Pack(dynBuf);
            }
            else
            {
                // Otherwise give back a VOTable
                dynBuf->Reset();

                if (calibratorList.GetVOTable(voHeader, softwareVersion, requestString, 
                                              xmlOutput.c_str(), dynBuf) == mcsFAILURE)
                {
                    TIMLOG_CANCEL(cmdName)
                }
            }
        }
    }
    else
    {
        if (dynBuf != NULL)
        {
            dynBuf->Reset();
        }
    }

    // Informing the request is completed
    if (_status.Write("0") == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Monitoring task is started only when msgMESSAGE is received.
    if (msg != NULL)
    {
        // Wait for the monitoring task end
        if (thrdThreadWait(&monitorTask) == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
    }

    // Stop timer log
    timlogStop(cmdName);
    
    return mcsSUCCESS;
}

/*___oOo___*/
