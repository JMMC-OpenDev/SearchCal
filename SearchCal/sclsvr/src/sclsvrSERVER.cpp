/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Definition of the sclsvrSERVER class.
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


/*
 * Local Headers
 */
#include "sclsvrCALIBRATOR.h"
#include "sclsvrSERVER.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrPrivate.h"
#include "sclsvrVersion.h"

/* initialize sclsvr module (vobsSTAR meta data) */
void sclsvrInit(bool loadJSDC)
{
    vobsPreInit();
    
    // first build star property index:
    sclsvrCalibratorBuildPropertyIndex();

    // prepare the catalog meta data (that use the property index):
    vobsInit();

    // Preload JSDC data:
    bool doQueryJSDC = false;

    /* load JSDC for web server */
    if (loadJSDC)
    {
        doQueryJSDC = sclsvrSCENARIO_JSDC_QUERY::loadData();
        
        if (!doQueryJSDC)
        {
            logWarning("Missing JSDC data: disabling scenario [JSDC QUERY]");
        }
    }
    sclsvrSERVER::SetQueryJSDC(doQueryJSDC);
    sclsvrSERVER::SetQueryJSDCFaint(doQueryJSDC);

    // Only allow JSDC build scenarios for internal usage:
    sclsvrSERVER::SetBuildJSDC(!loadJSDC);

    // dump server configuration at startup:
    sclsvrSERVER(mcsFALSE).DumpConfigAsXML();
}

/* clean sclsvr module on exit */
void sclsvrExit()
{
    vobsExit();

    // Free property meta data and index:
    vobsSTAR::FreePropertyIndex();
    sclsvrCALIBRATOR::FreePropertyIndex();

    // Free JSDC data:
    sclsvrSCENARIO_JSDC_QUERY::freeData();
}

/**
 * Monitor request execution status and forward it to the requester.
 *
 * @param param pointer to parameters needed by task.
 *
 * @return always NULL.
 */
thrdFCT_RET sclsvrMonitorTask(thrdFCT_ARG param)
{
    mcsSTRING256 buffer;
    mcsINT32 requestStatus = 1; // In progress

    // Get the server and message pointer back from the function parameter
    sclsvrMONITOR_TASK_PARAMS* taskParam = (sclsvrMONITOR_TASK_PARAMS*) param;
    sclsvrSERVER* server = (sclsvrSERVER*) taskParam->server;
    msgMESSAGE* message = (msgMESSAGE*) taskParam->message;
    sdbENTRY* status = (sdbENTRY*) taskParam->status;

    // Get current status and forward it to the GUI ...
    do
    {
        // Wait for a new status update
        if (status->Read(&buffer, mcsTRUE, 300) == mcsFAILURE)
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
            // Get catalog name, number and number of catalogs to be queried
            mcsSTRING256 catalogName;
            mcsINT32 catalogNum;
            mcsINT32 nbCatalogs;
            if (sscanf(buffer, "%*d\t%s\t%d\t%d",
                       catalogName, &catalogNum, &nbCatalogs) != 3)
            {
                logWarning("Wrong request execution status format");
                requestStatus = 0; // Assume request is completed
            }
            else
            {
                mcsSTRING256 reply;
                sprintf(reply, "Looking in '%s' catalog (%d/%d)...",
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
sclsvrSERVER::sclsvrSERVER(mcsLOGICAL unique) : evhSERVER(unique),
_virtualObservatory(),
_scenarioBrightK(&_status),
_scenarioJSDC(&_status),
_scenarioJSDC_Faint(&_status),
_scenarioJSDC_Query(&_status),
_scenarioBrightV(&_status),
_scenarioFaintK(&_status),
_scenarioSingleStar(&_status)
{
    _useVOStarListBackup = false;
    _working = false;

    // Create cancellation flag:
    _cancelFlag = new bool(false);
}

/*
 * Class destructor
 */
sclsvrSERVER::~sclsvrSERVER()
{
    // Free cancellation flag:
    delete _cancelFlag;
}

/*
 * Public methods
 */
mcsCOMPL_STAT sclsvrSERVER::AppInit()
{
    evhCMD_KEY key(sclsvrGETCAL_CMD_NAME, sclsvrGETCAL_CDF_NAME);
    evhCMD_CALLBACK cb(this, (evhCMD_CB_METHOD) & sclsvrSERVER::GetCalCB);
    AddCallback(key, cb);

    evhCMD_KEY key1(sclsvrGETSTAR_CMD_NAME, sclsvrGETSTAR_CDF_NAME);
    evhCMD_CALLBACK cb1(this, (evhCMD_CB_METHOD) & sclsvrSERVER::GetStarCB);
    AddCallback(key1, cb1);

    return mcsSUCCESS;
}

/**
 * Return the version number of the software.
 */
const char* sclsvrSERVER::GetSwVersion()
{
    return sclsvrVERSION;
}

// Dump the configuration as xml files

mcsCOMPL_STAT sclsvrSERVER::DumpConfigAsXML()
{
    // Build an empty request object
    sclsvrREQUEST request;
    request.SetSearchBand("K");
    request.SetSearchArea(1.0);

    miscoDYN_BUF xmlBuf;

    // A) Get Star Scenario:
    {
        vobsSTAR_LIST starList("GetStar");

        vobsSCENARIO::vobsSCENARIO_DumpXML = true;
        FAIL_DO(_scenarioSingleStar.DumpAsXML(xmlBuf, &_virtualObservatory, &request, &starList), 
                vobsSCENARIO::vobsSCENARIO_DumpXML = false);
        vobsSCENARIO::vobsSCENARIO_DumpXML = false;
    }

    // B) Get Cal Scenarii:

    // Bright K Scenario (I J H K):
    FAIL(_scenarioBrightK.DumpAsXML(xmlBuf, &_virtualObservatory, &request));

    // Bright V Scenario (V):
    FAIL(_scenarioBrightV.DumpAsXML(xmlBuf, &_virtualObservatory, &request));

    // JSDC Catalog Bright Scenario (0):
    FAIL(_scenarioJSDC.DumpAsXML(xmlBuf, &_virtualObservatory, &request));

    // JSDC Catalog Faint Scenario (0):
    FAIL(_scenarioJSDC_Faint.DumpAsXML(xmlBuf, &_virtualObservatory, &request));

    // JSDC Catalog Query Scenario (C):
    FAIL(_scenarioJSDC_Query.DumpAsXML(xmlBuf, &_virtualObservatory, &request));

    // Faint K Scenario (I J H K):
    vobsSCENARIO::vobsSCENARIO_DumpXML = true;
    FAIL_DO(_scenarioFaintK.DumpAsXML(xmlBuf, &_virtualObservatory, &request), 
            vobsSCENARIO::vobsSCENARIO_DumpXML = false);
    vobsSCENARIO::vobsSCENARIO_DumpXML = false;

    return mcsSUCCESS;
}


/*___oOo___*/
