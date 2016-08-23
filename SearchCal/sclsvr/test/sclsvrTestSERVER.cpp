/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Test program for sclsvrSERVER class
 */


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

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

/*
 * Local Headers 
 */
#include "sclsvr.h"
#include "sclsvrPrivate.h"

/* 
 * Local functions  
 */

/**
 * Structure used to pass two values as one paramter to a thrdTHREAD function.
 */
typedef struct
{
    // Request
    char* query;

    // Server instance
    sclsvrSERVER* server;

    // Result
    mcsCOMPL_STAT complStatus;
    miscoDYN_BUF* dynBuf;
} sclsvrGETCAL_TASK;

/**
 * Perfrom GETCAL request
 *
 * @param param a pointer on any data needed by the fuction.
 *
 * @return always NULL.
 */
thrdFCT_RET sclsvrGetCalTask(thrdFCT_ARG param)
{
    sclsvrGETCAL_TASK* taskParam = (sclsvrGETCAL_TASK*) param;
    char* query = (char*) taskParam->query;
    miscoDYN_BUF* dynBuf = (miscoDYN_BUF*) taskParam->dynBuf;
    sclsvrSERVER* server = (sclsvrSERVER*) taskParam->server;

    taskParam->complStatus = server->GetCal(query, dynBuf);

    return NULL;
}

/* 
 * Main
 */
int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Exit from the application with FAILURE
        exit(EXIT_FAILURE);
    }

    if (argc != 2)
    {
        printf("Usage : sclsvrTestSERVER <request>\n");
        exit(EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logQUIET);

    sclsvrSERVER server;
    miscoDYN_BUF dynBuf;
    // Fetch the resulting voTable
    // Thread parameters
    sclsvrGETCAL_TASK taskParams;
    taskParams.query = argv[1];
    taskParams.dynBuf = &dynBuf;
    taskParams.server = &server;

    // Thread creation and launch
    thrdTHREAD_STRUCT task;
    task.function = sclsvrGetCalTask;
    task.parameter = (thrdFCT_ARG*) & taskParams;

    // Launch the thread
    if (thrdThreadCreate(&task) == mcsFAILURE)
    {
        cout << "Thread creation failure !" << endl;
        return mcsFAILURE;
    }


    // Wait for each catalog...
    mcsSTRING256 status;
    mcsINT32 requestStatus = 1; // In progress
    do
    {
        if (server.GetStatus((char*) status) == mcsFAILURE)
        {
            errCloseStack();
            exit(EXIT_FAILURE);
        }

        // Status format is :
        //    reqStatus catalogName catalogNum nbCatalogs
        // Get request status
        if (sscanf(status, "%d", &requestStatus) != 1)
        {
            logWarning("Wrong request execution status format");
            requestStatus = 0; // Assume request is completed
        }
        // If execution is still in progress
        if (requestStatus == 1)
        {
            // Get catalog name, number and number of catalogs to be consulted
            mcsSTRING256 catalogName;
            mcsINT32 catalogNum;
            mcsINT32 nbCatalogs;
            if (sscanf(status, "%*d\t%s\t%d\t%d",
                       catalogName, &catalogNum, &nbCatalogs) != 3)
            {
                logWarning("Wrong request execution status format");
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

    // Wait for the thread end
    if (thrdThreadWait(&task) == mcsFAILURE)
    {
        exit(EXIT_FAILURE);
    }

    if (taskParams.complStatus == mcsSUCCESS)
    {
        miscDynSIZE resultSize;
        taskParams.dynBuf->GetNbStoredBytes(&resultSize);
        cout << "Resulting VOTable (size = '" << resultSize << "') :" << endl;
        cout << taskParams.dynBuf->GetBuffer() << endl;
    }
    else
    {
        errCloseStack();
        exit(EXIT_FAILURE);
    }
    // Close MCS services
    mcsExit();

    // Exit from the application with SUCCESS
    exit(EXIT_SUCCESS);
}


/*___oOo___*/
