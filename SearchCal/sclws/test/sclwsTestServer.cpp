/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Test program implenting a clinet to web service
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>
#include <signal.h>

/**
 * @namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "thrd.h"
#include "timlog.h"

/*
 * Local Headers 
 */
#include "soapH.h"
#include "soap.nsmap"
#include "sclwsPrivate.h"
#include "sclwsErrors.h"

/*
 * Local Variables
 */
struct Namespace *namespaces;
mcsSTRING256 sclwsURL;
typedef struct
{
    // Request
    char* query;

    // Web-service Id
    char* taskId;

    // Result; i.e. the VO Table
    mcsCOMPL_STAT complStatus;
    std::string   result;
} sclwsGETCAL_TASK;

char* sclwsTaskId;

/*
 * Local Functions
 */
void sclwsThreadStats(mcsUINT32 *threadCreated, mcsUINT32 *threadJoined)
{
// Not implemented function
}

/**
 * Monitor any action and forward it to the shell.
 *
 * @param param a pointer on any data needed by the fuction.
 *
 * @return always NULL.
 */
thrdFCT_RET sclwsGetCalTask(thrdFCT_ARG param)
{   
    // Define a new SOAP context for the thread (can run in parallel of main())
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Retrieve parameters
    sclwsGETCAL_TASK* taskParam = (sclwsGETCAL_TASK*)param;
    char*  taskId  = taskParam->taskId;
    char*  query   = taskParam->query;
    char*  result  = NULL;

    // Start timer log
    timlogInfoStart("GETCAL");

    // Launch the GETCAL query
    soap_call_ns__GetCalSearchCal(&v_soap, sclwsURL, "", taskId, query, &result);

    // Stop timer log
    timlogStop("GETCAL");

    // Check completion status
    if (v_soap.error)
    {
        taskParam->complStatus = mcsFAILURE;
        taskParam->result = v_soap.fault->faultstring; 

        soap_print_fault(&v_soap, stderr);
    }
    else
    {
        taskParam->complStatus = mcsSUCCESS;
        taskParam->result = result;
    }
    
    soap_end(&v_soap);
    return NULL;
}


/* 
 * Signal catching functions  
 */
void sclwsSignalHandler (int signalNumber)
{
    logInfo("Received a '%d' system signal ...", signalNumber);

    if (signalNumber == SIGPIPE)
    {
        return;
    }

    // Cancelling the query
    logInfo("Cancelling the query.");
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);
    bool  isOk  = false;
    soap_call_ns__GetCalCancelSession(&v_soap, sclwsURL, "", sclwsTaskId, &isOk);
    if (v_soap.error)
    {
        soap_print_fault(&v_soap, stderr);
        exit (EXIT_FAILURE);
    }
    logInfo("Cancel status = '%d'.", isOk);

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/* 
 * Main
 */
int main(int argc, char *argv[])
{
    /* Init system signal trapping */
    if (signal(SIGINT, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGINT, ...) function error");
        exit (EXIT_FAILURE);
    }
    if (signal(SIGTERM, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGTERM, ...) function error");
        exit (EXIT_FAILURE);
    }
    if (signal(SIGPIPE, sclwsSignalHandler) == SIG_ERR)
    {
        logError("signal(SIGPIPE, ...) function error");
        exit (EXIT_FAILURE);
    }

    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    // Set stdout log level 
    logSetStdoutLogLevel(logINFO);

    // Composing server address
    char* url = "http://jmmc.fr";
    mcsUINT16 portNumber = sclwsGetServerPortNumber();
    snprintf(sclwsURL, sizeof(sclwsURL), "%s:%d", url, portNumber);

    // Define a SOAP context for main()
    struct soap v_soap;
    soap_init(&v_soap);
    soap_set_namespaces(&v_soap, soap_namespaces);

    // Fetch the a CGETCAL WebService communication ID
    soap_call_ns__GetCalOpenSession(&v_soap, sclwsURL, "", &sclwsTaskId);
    if (v_soap.error)
    {
        soap_print_fault(&v_soap, stderr);
        exit (EXIT_FAILURE);
    }
    logInfo("Connection to '%s' established (ID = '%s').", sclwsURL, sclwsTaskId);

    // Launch the thread that handle the GETCAL query
    // Thread parameters creation
    sclwsGETCAL_TASK      taskParams;
    taskParams.taskId  = sclwsTaskId;
    taskParams.query   = argv[1];
    // Thread creation and launch
    thrdTHREAD_STRUCT      getCalTask;
    getCalTask.function  = sclwsGetCalTask;
    getCalTask.parameter = (thrdFCT_ARG*)&taskParams;
    // Launch the thread
    if (thrdThreadCreate(&getCalTask) == mcsFAILURE)
    {
        errAdd(sclwsERR_THREAD_CREATION);
        exit (EXIT_FAILURE);
    }

    // Fetch each queried catalog name to monitor query progress
    char*      status  = NULL;
    mcsINT32   requestStatus = 1; // In progress
    do
    {
        // Ask and wait for the currently quereid catalog name
        logDebug("Waiting for the Catalog Name.");
        soap_call_ns__GetCalQueryStatus(&v_soap, sclwsURL, "", 
                                        sclwsTaskId, &status);
        if (v_soap.error)
        {
            soap_print_fault(&v_soap, stderr);
            exit (EXIT_FAILURE);
        }

        // Status format is :
        //    reqStatus catalogName catalogNum nbCatalogs
        // Get request status
        if (sscanf(status, "%d", &requestStatus) != 1)
        {
            logWarning("Wrong request execution status format - %s",
                       status);
            requestStatus = 0; // Assume request is completed
        }
        // If execution is still in progress
        if (requestStatus == 1)
        {
            // Get catalog name, number and number of catalogs to be consulted
            mcsSTRING256 catalogName;
            mcsINT32     catalogNum;
            mcsINT32     nbCatalogs;
            if (sscanf(status, "%*d\t%s\t%d\t%d", 
                       catalogName, &catalogNum, &nbCatalogs) != 3)
            {
                logWarning("Wrong request execution status format - %s",
                           status);
                requestStatus = 0; // Assume request is completed
            }
            if (strlen(status) != 0)
            {
                cout << "Looking in '" << catalogName << "' catalog ";
                cout << catalogNum << "/" << nbCatalogs << "..." << endl;
            }
        }
    }
    while (requestStatus == 1); // ... until the request is completed 

    // Wait for the thread end (eg. result receiving)
    logDebug("Waiting for thread termination");
    if (thrdThreadWait(&getCalTask) == mcsFAILURE)
    {
        errAdd(sclwsERR_THREAD_CREATION);
        errCloseStack();
        exit (EXIT_FAILURE);
    }
    logDebug("Thread terminated");

    if (taskParams.complStatus == mcsFAILURE)
    {
        logError("ERROR: %s", taskParams.result.c_str());
    }
    else
    {
        if (taskParams.result.length() == 0)
        {
            logWarning("No star found");
        }
        else
        {
            // Output the received VO Table on stdout
            cout << taskParams.result << endl;
        }
    }

    // Dealloc SOAP context
    logDebug("Disconnecting from '%s' (ID = '%s').", sclwsURL, sclwsTaskId);
    soap_end(&v_soap);
    logInfo("Connection closed.");

    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
