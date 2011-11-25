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
extern "C"{
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
    logTrace("sclsvrSERVER::GetStarCB()");

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
    logTrace("sclsvrSERVER::GetStar()");

    // Get calibrators
    evhCB_COMPL_STAT complStatus = ProcessGetStarCmd(query, dynBuf, NULL);

    // Update status to inform request processing is completed 
    if (_status.Write("0") == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (complStatus != evhCB_SUCCESS)
    {
        return mcsFAILURE;
    }
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
    logTrace("sclsvrSERVER::ProcessGetStarCmd()");

    static const char* cmdName = "GETSTAR";

    // Search command
    sclsvrGETSTAR_CMD getStarCmd(cmdName, query);
    
    // Get the request as a string for the case of Save in VOTable
    mcsSTRING256 requestString;
    strncpy(requestString, cmdName, 256);

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

        // Launch the thread only if SDB had been succesfully started
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
    char* fileName;
    if (getStarCmd.GetFile(&fileName) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }    

    // Get observed wavelength 
    double wlen;
    if (getStarCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }      

    // Get baseline 
    double baseline;
    if (getStarCmd.GetWlen(&baseline) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }        
    
    // Get star position from SIMBAD
    mcsSTRING32 ra, dec;
    if (simcliGetCoordinates(objectName, ra, dec) == mcsFAILURE)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, objectName, "SIMBAD");

        TIMLOG_CANCEL(cmdName)
    }

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
    if (request.SetFileName(fileName) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    if (request.SetSearchBand("K") ==  mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    if (request.SetObservingWlen(wlen) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }
    if (request.SetMaxBaselineLength(baseline) ==  mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    // Set star
    vobsSTAR star;
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN,  request.GetObjectRa(),  vobsSTAR_UNDEFINED);
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, request.GetObjectDec(), vobsSTAR_UNDEFINED);
    
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    
    // init the scenario
    if (_scenarioSingleStar.Init(&request, starList) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
    }

    if (_virtualObservatory.Search(&_scenarioSingleStar, request, starList) == mcsFAILURE)
    {
        TIMLOG_CANCEL(cmdName)
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

        if (_status.Write("Done") == mcsFAILURE)
        {
            TIMLOG_CANCEL(cmdName)
        }
        //
	// Complete missing properties of the calibrator 
	if (calibrator.Complete(request) == mcsFAILURE)
	  {
	    // Ignore error
	    errCloseStack(); 
	  }

        // Prepare reply
        if (dynBuf != NULL)
        {
            int propIdx;
            int propLen = calibrator.NbProperties();
            vobsSTAR_PROPERTY *property;
            
            // Add property name
            for (propIdx = 0; propIdx < propLen; propIdx++)
            {
                property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
                dynBuf->AppendString(property->GetName());
                dynBuf->AppendString("\t");
            }
            dynBuf->AppendString("\n");
            
            // Add property value
            for (propIdx = 0; propIdx < propLen; propIdx++)
            {
                property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
                dynBuf->AppendString(property->GetValue());
                dynBuf->AppendString("\t");
            }
            dynBuf->AppendString("\n");

            // Send reply
            if (msg != NULL)
            {
                msg->SetBody(dynBuf->GetBuffer());
            }
            else
            {
                printf("%s", dynBuf->GetBuffer());
            }
            dynBuf->Reset();
        }

        string xmlOutput;
        //request.AppendParamsToVOTable(xmlOutput);
        const char* voHeader = "Produced by beta version of getStar (In case of problem, please report to jmmc-user-support@ujf-grenoble.fr)";
        
        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof(softwareVersion), "%s v%s", "sclsvr", sclsvrVERSION);

        // If a filename has been given, store results as file
        if (strcmp(request.GetFileName(), "") != 0)
        {
            vobsSTAR_LIST newStarList;
            newStarList.AddAtTail(calibrator);
            
            // Save the list as a VOTable v1.1
            if (newStarList.SaveToVOTable(request.GetFileName(), voHeader, softwareVersion,
                                          requestString, xmlOutput.c_str()) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }
        }

        // Send reply
        if (msg != NULL)
        {
            if (SendReply(*msg) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }
            // Wait for the actionForwarder thread end
            if (thrdThreadWait(&monitorTask) == mcsFAILURE)
            {
                TIMLOG_CANCEL(cmdName)
            }
        }
    }

    if (msg != NULL)
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
