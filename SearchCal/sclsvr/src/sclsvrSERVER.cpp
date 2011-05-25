/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of the sclsvrSERVER class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSERVER.cpp,v 1.16.2.1 2011-04-15 22:47:18 duvert Exp $"; 


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


/*
 * Local Headers 
 */
#include "sclsvrSERVER.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrPrivate.h"
#include "sclsvrVersion.h"


/**
 * Monitor request execution status and forward it to the requester.
 *
 * @param param pointer to parameters needed by task.
 *
 * @return always NULL.
 */
thrdFCT_RET sclsvrMonitorTask(thrdFCT_ARG param)
{   
    logTrace("sclsvrMonitorTask()");
    mcsSTRING256  buffer;
    mcsINT32      requestStatus = 1; // In progress

    // Get the server and message pointer back from the function parameter
    sclsvrMONITOR_TASK_PARAMS* taskParam = (sclsvrMONITOR_TASK_PARAMS*) param;
    sclsvrSERVER*                 server = (sclsvrSERVER*) taskParam->server;
    msgMESSAGE*                  message = (msgMESSAGE*) taskParam->message;
    sdbENTRY*                     status = (sdbENTRY*) taskParam->status;

    // Get current status and forward it to the GUI ...
    do
    {
        // Wait for a new status update
        if (status->Read(buffer, mcsTRUE, 300) == mcsFAILURE)
        {
            return NULL;
        }

        // Status format is :
        //    reqStatus catalogName catalogNum nbCatalogs
        // Get request status
        if (sscanf(buffer, "%d", &requestStatus) != 1)
        {
            logWarning("Wrong request execution status format");
            requestStatus = 0; // Assume request is completed
        }

        // If execution is still in progress
        if (requestStatus == 1)
        {
            // Get catalog name, number and number of catalogs to be consulted
            mcsSTRING256 catalogName;
            mcsINT32     catalogNum;
            mcsINT32     nbCatalogs;
            if (sscanf(buffer, "%*d\t%s\t%d\t%d", 
                       catalogName, &catalogNum, &nbCatalogs) != 3)
            {
                logWarning("Wrong request execution status format");
                requestStatus = 0; // Assume request is completed
            }
            else
            {
                mcsSTRING256 reply;
                sprintf (reply, "Looking in '%s' catalog (%d/%d)...",
                         catalogName, catalogNum, nbCatalogs);

                // Send current status
                if (message->SetBody(reply) == mcsFAILURE)
                {
                    return NULL;
                }

                // Send the new message to the GUI for status display
                if (server->SendReply(*message, mcsFALSE) == mcsFAILURE)
                {
                    return NULL;
                }
            }
        }
    }
    while (requestStatus == 1); // ... until the request is completed 

    return NULL;
}

/*
 * Class constructor
 */
sclsvrSERVER::sclsvrSERVER():
    _virtualObservatory(),
    _scenarioBrightK(&_status),
    _scenarioJSDC(&_status),
    _scenarioBrightKCatalog(&_status),
    _scenarioBrightV(&_status),
    _scenarioBrightN(&_status),
    _scenarioFaintK(&_status),
    _scenarioSingleStar(&_status)
{
}

/*
 * Class destructor
 */
sclsvrSERVER::~sclsvrSERVER()
{
}

/*
 * Public methods
 */
mcsCOMPL_STAT sclsvrSERVER::AppInit()
{
    logTrace("sclsvrSERVER::AppInit()");

    evhCMD_KEY key(sclsvrGETCAL_CMD_NAME, sclsvrGETCAL_CDF_NAME);
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetCalCB);
    AddCallback(key, cb);
    
    evhCMD_KEY key1(sclsvrGETSTAR_CMD_NAME, sclsvrGETSTAR_CDF_NAME);
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD)&sclsvrSERVER::GetStarCB);
    AddCallback(key1, cb1);
    
    return mcsSUCCESS;
}

/**
 * Return the version number of the software.
 */
const char *sclsvrSERVER::GetSwVersion()
{
    return sclsvrVERSION;
}
/*___oOo___*/
