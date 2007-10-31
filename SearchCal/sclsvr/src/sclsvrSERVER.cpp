/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSERVER.cpp,v 1.15 2007-10-31 11:35:18 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.14  2007/05/15 08:37:16  gzins
 * Fixed bug related to thread synchronisation
 *
 * Revision 1.13  2007/02/09 17:04:06  lafrasse
 * Moved _progress deletion from GetCalCB() to sclsvrSERVER destructor.
 *
 * Revision 1.12  2007/02/04 20:45:53  lafrasse
 * Added GetCatalogIndex() API to get the index of the catalog being queried.
 * Added GetNbOfCatalogs() API to get the number of catalogs of the scenario.
 *
 * Revision 1.11  2007/01/23 14:29:45  lafrasse
 * Removed unwanted debugging traces.
 *
 * Revision 1.10  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.9  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.8  2006/02/21 16:52:39  scetre
 * Moved the 2 same method in one in sclsvrSERVER.cpp
 * move the 2 same struct in sclsvrPrivate.h
 *
 * Revision 1.7  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.6  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.5  2005/03/06 20:29:40  gzins
 * Added GetSwVersion method
 *
 * Revision 1.4  2005/01/26 13:13:04  gzins
 * Added CVS log as modification history.
 * Added CDF file when attaching command callbacks.
 *
 ******************************************************************************/

/**
 * @file
 * Definition of the sclsvrSERVER class.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSERVER.cpp,v 1.15 2007-10-31 11:35:18 gzins Exp $"; 


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
