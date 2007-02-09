/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrSERVER.cpp,v 1.13 2007-02-09 17:04:06 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrSERVER.cpp,v 1.13 2007-02-09 17:04:06 lafrasse Exp $"; 


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
 * Monitor any action and forward it to the shell.
 *
 * @param param a pointer on any data needed by the fuction.
 *
 * @return always NULL.
 */
thrdFCT_RET sclsvrMonitorAction(thrdFCT_ARG param)
{   
    logTrace("sclsvrMonitorAction()");

    mcsSTRING256  buffer;
    mcsLOGICAL    lastMessage = mcsFALSE;

    // Get the server and message pointer back from the function parameter
    sclsvrMonitorActionParams* paramsPtr = (sclsvrMonitorActionParams*) param;
    sclsvrSERVER*                 server = (sclsvrSERVER*) paramsPtr->server;
    msgMESSAGE*                  message = (msgMESSAGE*) paramsPtr->message;
    sdbENTRY*         progressionMessage = (sdbENTRY*) paramsPtr->progressionMessage;

    // Get any new action and forward it to the GUI ...
    do
    {
        // Wait for a new action
        if (progressionMessage->Wait(buffer, &lastMessage) == mcsFAILURE)
        {
            return NULL;
        }

        // Define the new message body from the newly received action message
        if (message->SetBody(buffer) == mcsFAILURE)
        {
            return NULL;
        }

        // Send the new message to the GUI for status display
        if (server->SendReply(*message, mcsFALSE) == mcsFAILURE)
        {
            return NULL;
        }
    }
    while (lastMessage == mcsFALSE); // ... until the last action occured

    return NULL;
}


/*
 * Class constructor
 */
sclsvrSERVER::sclsvrSERVER():
_virtualObservatory(),
_scenarioBrightK(&_progress),
_scenarioBrightKOld(&_progress),
_scenarioBrightV(&_progress),
_scenarioBrightN(&_progress),
_scenarioFaintK(&_progress),
_scenarioSingleStar(&_progress)
{
    // sdbAction initialization
    _progressionMessageInitFlag = mcsFALSE;
    if (_progress.Init() == mcsSUCCESS)
    {
        _progressionMessageInitFlag = mcsTRUE;
    }
    else
    {
        _progressionMessageInitFlag = mcsFALSE;
        errCloseStack();
    }

    _selectedScenario = NULL;
    _lastCatalog  = mcsFALSE;
}

/*
 * Class destructor
 */
sclsvrSERVER::~sclsvrSERVER()
{
    _progress.Destroy();
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
 * Return the total number of catalog queried by the scenario.
 *
 * @return an mcsUINT32 
 */
mcsUINT32 sclsvrSERVER::GetNbOfCatalogs()
{
    logTrace("sclsvrSERVER::GetNbOfCatalogs()");

    if (_selectedScenario != NULL)
    {
        return _selectedScenario->GetNbOfCatalogs();
    }

    return 0;
}
 
/**
 * Return the current index of the catalog being queried.
 *
 * @return an mcsUINT32 
 */
mcsUINT32 sclsvrSERVER::GetCatalogIndex()
{
    logTrace("sclsvrSERVER::GetCatalogIndex()");

    if (_selectedScenario != NULL)
    {
        return _selectedScenario->GetCatalogIndex();
    }

    return 0;
}
 
/**
 * Return the version number of the software.
 */
const char *sclsvrSERVER::GetSwVersion()
{
    return sclsvrVERSION;
}


/*
 * Protected methods
 */


/*
 * Private methods
 */


/*___oOo___*/
