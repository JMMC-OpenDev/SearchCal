/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetCalCB.cpp,v 1.26 2005-12-21 10:32:56 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.25  2005/12/12 14:10:30  scetre
 * Added old scenario with 2mass in primary request if precised in the GETCAL command
 *
 * Revision 1.24  2005/11/24 15:14:27  scetre
 * Scenario faint K is enable
 *
 * Revision 1.23  2005/11/24 09:00:10  lafrasse
 * Added 'radius' parameter to the GETCAL command
 *
 * Revision 1.22  2005/11/23 14:35:33  lafrasse
 * Added fileName proper management (strncpy() calls instead of strcpy())
 * Removed unused 'MaxReturn' command parmater
 * Added 'bright' command parameter
 *
 * Revision 1.21  2005/11/21 13:50:59  scetre
 * Changed bad scenario name
 *
 * Revision 1.20  2005/11/15 15:01:19  scetre
 * Updated with new scenario structure
 *
 * Revision 1.19  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.18  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.17  2005/03/06 20:31:13  gzins
 * Updated sclsvrCALIBRATOR_LIST::Save() API
 *
 * Revision 1.16  2005/03/04 15:38:10  gzins
 * Added timer log
 *
 * Revision 1.15  2005/03/04 09:47:17  gzins
 * Implemented -file option; i.e. save calibrator list in file if requested
 *
 * Revision 1.14  2005/02/16 16:56:30  gzins
 * Fixed wrong parameter name in documentation
 *
 * Revision 1.13  2005/02/14 15:10:40  scetre
 * changed m..RangeMag to m..MagRange
 *
 * Revision 1.12  2005/02/14 14:13:41  scetre
 * minor changed
 *
 * Revision 1.11  2005/02/13 16:08:09  gzins
 * Changed Min/MaxDeltaMag to Min/MaxRangeMag
 *
 * Revision 1.10  2005/02/08 20:55:50  gzins
 * Removed display of resulting star list
 *
 * Revision 1.9  2005/02/08 04:39:32  gzins
 * Updated for new vobsREQUEST API and used new sclsvrREQUEST class
 *
 * Revision 1.8  2005/02/07 15:01:11  gzins
 * Reformated file header
 *
 * Revision 1.7  2005/02/0sclsvrSERVER.cpp7 14:44:03  gzins
 * Renamed lambda to wlen.
 * Renamed minMagRange to minRangeMag, and maxMagRange to maxRangeMag.
 *
 * Revision 1.6  2005/02/07 09:24:42  gzins
 * Replaced vobsVIRTUAL_OBSERVATORY local instance by the class member
 *
 * Revision 1.5  2005/02/04 15:29:54  gzins
 * Removed unused printf
 *
 * gzins     23-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrGetCalCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetCalCB.cpp,v 1.26 2005-12-21 10:32:56 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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


/*
 * Local Headers 
 */
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_BRIGHT_N.h"


/*
 * Local structure
 */
/**
 * Structure used to pass two values as one paramter to a thrdTHREAD function.
 */
typedef struct
{
    sclsvrSERVER*  server;   /**< pointer on a sclsvrSERVER instance. */

    msgMESSAGE*    message;  /**< pointer on a msgMESSAGE instance. */

} sclsvrMonitorActionParams;

/*
 * Local functions
 */
/**
 * Monitor any action and forward it to the GUI as status.
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
    sclsvrMonitorActionParams* paramsPtr = (sclsvrMonitorActionParams*)param;
    sclsvrSERVER*                 server = (sclsvrSERVER*)paramsPtr->server;
    msgMESSAGE*                  message = (msgMESSAGE*) paramsPtr->message;

    // Get any new action and forward it to the GUI ...
    do
    {
        // Wait for a new action
        if (sdbWaitAction(buffer, &lastMessage) == mcsFAILURE)
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
 * Public methods
 */
evhCB_COMPL_STAT sclsvrSERVER::GetCalCB(msgMESSAGE &msg, void*)
{
    logTrace("sclsvrSERVER::GetCalCB()");

    // Build the request object from the parameters of the command
    sclsvrREQUEST request;
    if (request.Parse(msg.GetBody()) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
 
    // Start timer log
    timlogInfoStart(msg.GetCommand());

    // sdbAction initialization
    sdbInitAction();

    // actionMonitor thread parameters creation
    sclsvrMonitorActionParams      actionMonitorParams;
    actionMonitorParams.server   = this;
    actionMonitorParams.message  = &msg;

    // actionMonitor thread creation and launch
    thrdTHREAD                     actionMonitor;
    actionMonitor.function       = sclsvrMonitorAction;
    actionMonitor.parameter      = (thrdFCT_ARG*)&actionMonitorParams;
    thrdThreadCreate(&actionMonitor);

    // Build the list of star which will come from the virtual observatory
    vobsSTAR_LIST starList;

    // If the request should return bright starts
    if ((request.IsBright() == mcsTRUE) &&
        (request.GetSearchAreaGeometry() == vobsBOX))
    {
        // According to the desired band
        const char* band = request.GetSearchBand();
        switch(band[0])
        {
            case 'I':
            case 'J':
            case 'H':
            case 'K':
                if (request.IsOldScenario() == mcsTRUE)
                {
                    // Load old Bright K Scenario
                    if (_scenarioBrightKOld.Init(&request) == mcsFAILURE)
                    {
                        return evhCB_NO_DELETE | evhCB_FAILURE;
                    }
                    // Start the research in the virtual observatory
                    if (_virtualObservatory.Search(&_scenarioBrightKOld,
                                                   request,
                                                   starList) == mcsFAILURE)
                    {
                        return evhCB_NO_DELETE | evhCB_FAILURE;
                    }

                }
                else
                {
                    // Load Bright K Scenario
                    if (_scenarioBrightK.Init(&request) == mcsFAILURE)
                    {
                        return evhCB_NO_DELETE | evhCB_FAILURE;
                    }
                    // Start the research in the virtual observatory
                    if (_virtualObservatory.Search(&_scenarioBrightK, request,
                                                   starList) == mcsFAILURE)
                    {
                        return evhCB_NO_DELETE | evhCB_FAILURE;
                    }
                }

                break;

            case 'V':
                // Load Bright V Scenario
                if (_scenarioBrightV.Init(&request) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                // Start the research in the virtual observatory
                if (_virtualObservatory.Search(&_scenarioBrightV, request,
                                               starList) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                break;

            case 'N':
                // Load Bright N Scenario
                if (_scenarioBrightN.Init(&request) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                // Start the research in the virtual observatory
                if (_virtualObservatory.Search(&_scenarioBrightN, request,
                                               starList) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                break;

            default:
                errAdd(vobsERR_UNKNOWN_BAND, band);
                return evhCB_NO_DELETE | evhCB_FAILURE;
                break;
        }
    }
    else if ((request.IsBright() == mcsFALSE) &&
             (request.GetSearchAreaGeometry() == vobsCIRCLE))
    {
        // According to the desired band
        const char* band = request.GetSearchBand();
        switch(band[0])
        {
            case 'K':
                // Load Faint K Scenario
                if (_scenarioFaintK.Init(&request) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                // Start the research in the virtual observatory
                if (_virtualObservatory.Search(&_scenarioFaintK, request,
                                               starList) == mcsFAILURE)
                {
                    return evhCB_NO_DELETE | evhCB_FAILURE;
                }

                break;

            default:
                errAdd(vobsERR_UNKNOWN_BAND, band);
                return evhCB_NO_DELETE | evhCB_FAILURE;
                break;
        }
    }
    else
    {
        logError("Invalid query.");

        if (request.IsBright() == mcsTRUE)
        {
            logError("Bright query.");
        }
        else
        {
            logError("Faint query.");
        }

        if (request.GetSearchAreaGeometry() == vobsBOX)
        {
            logError("Rectangular query.");
        }
        else
        {
            logError("Circular query.");
        }

        return evhCB_NO_DELETE;
    }

    // Build the list of calibrator
    sclsvrCALIBRATOR_LIST calibratorList;
   
    // Get the returned star list and create a calibrator list from it
    if (calibratorList.Copy(starList) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    sdbWriteAction("Completing results...", mcsFALSE);

    // Complete the calibrators list
    if (calibratorList.Complete(request) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    sdbWriteAction("Done", mcsTRUE);

    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    { 
        miscoDYN_BUF dynBuff;
        calibratorList.Pack(&dynBuff);
        
        msg.SetBody(dynBuff.GetBuffer());

        // If a file has been given, store result in this file
        if (strcmp(request.GetFileName(), "") != 0)
        {
            if (calibratorList.Save(request.GetFileName(), 
                                    request) == mcsFAILURE)
            {
                return evhCB_NO_DELETE | evhCB_FAILURE;
            };
        }
    }
    else
    {
        msg.ClearBody();
    }

    // Wait for the actionForwarder thread end
    thrdThreadWait(&actionMonitor);

    // Send reply
    if (SendReply(msg) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Stop timer log
    timlogStop(msg.GetCommand());
    
    return evhCB_NO_DELETE;
}


/*___oOo___*/
