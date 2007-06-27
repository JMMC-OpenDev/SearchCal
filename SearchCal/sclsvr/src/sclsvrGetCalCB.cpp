/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetCalCB.cpp,v 1.51 2007-06-27 14:26:49 scetre Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.50  2007/06/27 13:00:59  scetre
 * Do not removed science star if present in the resulting list.
 * Updated get star command
 *
 * Revision 1.49  2007/05/15 08:37:16  gzins
 * Fixed bug related to thread synchronisation
 *
 * Revision 1.48  2007/04/27 09:04:03  gzins
 * Returned empty buffer instead of error when no calibrator has been found
 *
 * Revision 1.47  2007/02/09 17:04:06  lafrasse
 * Moved _progress deletion from GetCalCB() to sclsvrSERVER destructor.
 *
 * Revision 1.46  2007/02/04 20:49:42  lafrasse
 * Simplified scenario selection API with polymorphism.
 * Replaced GetCalStatus() by WaitForCurrentCatalogName() and IsLastCatalog().
 *
 * Revision 1.45  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.44  2006/10/26 08:15:57  gzins
 * Renamed thrdTHREAD to thrdTHREAD_STRUCT
 *
 * Revision 1.43  2006/10/10 15:50:44  lafrasse
 * Changed XML Serialization in VOTable PARAM.
 *
 * Revision 1.42  2006/10/10 11:30:18  lafrasse
 * Changed request XML serialization APIs in SerializeToXML().
 *
 * Revision 1.41  2006/10/09 15:07:07  lafrasse
 * Added request XML serialization in VOTables.
 *
 * Revision 1.40  2006/08/25 06:07:02  gzins
 * Fixed bug related to error handling; vobsERR_xxx errors was added in error stack.
 *
 * Revision 1.39  2006/08/23 12:07:43  gzins
 * When not given, set radius to 0 in faint scenario
 *
 * Revision 1.38  2006/07/17 09:10:36  scetre
 * Added old scenario option
 *
 * Revision 1.37  2006/07/04 10:18:00  scetre
 * Managed the format of the file for the save in command. Actually, it is possible to save in votable if extension of the -file option is .vot, otherwise standard format
 *
 * Revision 1.36  2006/04/05 15:17:26  gzins
 * Added message when science star is removed form list
 *
 * Revision 1.35  2006/03/07 15:33:39  scetre
 * Removed old scenario in band K
 *
 * Revision 1.34  2006/03/07 07:52:57  scetre
 * Added error instead of logError
 *
 * Revision 1.33  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.32  2006/03/01 16:52:44  lafrasse
 * Added code to remove the science object if it belongs to the calibrator list.
 *
 * Revision 1.31  2006/02/23 16:08:21  lafrasse
 * Added error stack purge in case of bad 'sdb' initialization
 *
 * Revision 1.30  2006/02/22 17:08:33  lafrasse
 * Made it works even if there is no more semaphores left on the running system (no GUI progression in this case)
 *
 * Revision 1.29  2006/02/21 16:52:39  scetre
 * Moved the 2 same method in one in sclsvrSERVER.cpp
 * move the 2 same struct in sclsvrPrivate.h
 *
 * Revision 1.28  2005/12/22 14:39:38  scetre
 * Added sdb and thrd in GetStar
 *
 * Revision 1.27  2005/12/22 14:12:14  lafrasse
 * Added error managment code and sdbDestroyAction() call to ensure that any created semaphores are released properly
 *
 * Revision 1.26  2005/12/21 10:32:56  lafrasse
 * Added a querying actions monitoring thread to forward them to the GUI as status
 *
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

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrGetCalCB.cpp,v 1.51 2007-06-27 14:26:49 scetre Exp $"; 


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
#include "sclsvrVersion.h"
#include "sclsvrErrors.h"
#include "sclsvrSERVER.h"
#include "sclsvrPrivate.h"
#include "sclsvrCALIBRATOR_LIST.h"
#include "sclsvrSCENARIO_BRIGHT_K.h"
#include "sclsvrSCENARIO_BRIGHT_V.h"
#include "sclsvrSCENARIO_BRIGHT_N.h"


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
mcsCOMPL_STAT sclsvrSERVER::WaitForCurrentCatalogName(char* buffer)
{
    logTrace("sclsvrSERVER::WaitForCurrentCatalogName()");

    // Wait for a new action
    if (_progress.Wait(buffer, &_lastCatalog) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    return mcsSUCCESS;
}

mcsLOGICAL sclsvrSERVER::IsLastCatalog()
{
    logTrace("sclsvrSERVER::IsLastCatalog()");

    return _lastCatalog;
}

mcsCOMPL_STAT sclsvrSERVER::GetCal(const char* query, miscoDYN_BUF &dynBuff, 
                                   msgMESSAGE* msg = NULL)
{
    logTrace("sclsvrSERVER::GetCal()");

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
    timlogInfoStart("GETCAL");
    
    // actionMonitor thread parameters creation
    sclsvrMonitorActionParams                  actionMonitorParams;
    actionMonitorParams.server              =  this;
    actionMonitorParams.message             =  msg;
    actionMonitorParams.progressionMessage  = &_progress;

    // actionMonitor thread creation and launch
    thrdTHREAD_STRUCT              actionMonitor;
    actionMonitor.function       = sclsvrMonitorAction;
    actionMonitor.parameter      = (thrdFCT_ARG*)&actionMonitorParams;

    // sdbAction init
    if (_progress.Init() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // SDB starting
    if (_progress.IsInit() == mcsTRUE)
    {
        // Status monitoring should be done by thread with msgMESSAGE reply.
        if (msg != NULL)
        {
            // Launch the status monitoring thread
            if (thrdThreadCreate(&actionMonitor) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    // Build the list of star which will come from the virtual observatory
    vobsSTAR_LIST starList;
    _selectedScenario = NULL;

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
                    _selectedScenario = &_scenarioBrightKOld;
                }
                else
                {
                    // Load Bright K Scenario
                    _selectedScenario = &_scenarioBrightK;
                }
                break;

            case 'V':
                // Load Bright V Scenario
                _selectedScenario = &_scenarioBrightV;
                break;

            case 'N':
                // Load Bright N Scenario
                _selectedScenario = &_scenarioBrightN;
                break;

            default:
                errAdd(sclsvrERR_UNKNOWN_BAND, band);
                return mcsFAILURE;
                break;
        }
    }
    else if ((request.IsBright() == mcsFALSE))
    {
        // If radius has not been given, set it to 0; i.e. determine by SW
        if (request.GetSearchAreaGeometry() != vobsCIRCLE) 
        {
            request.SetSearchArea(0.0);
        }

        // According to the desired band
        const char* band = request.GetSearchBand();
        switch(band[0])
        {
            case 'K':
                // Load Faint K Scenario
                _selectedScenario = &_scenarioFaintK;
                break;

            default:
                errAdd(sclsvrERR_UNKNOWN_BAND, band);
                return mcsFAILURE;
                break;
        }
    }
    else
    {        
        if (request.GetSearchAreaGeometry() == vobsCIRCLE)
        {
            errAdd(sclsvrERR_INVALID_SEARCH_AREA, "bright", "rectangular");
        }
 
        return mcsFAILURE;
    }

    // Load the scenario
    if (_selectedScenario->Init(&request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Start the research in the virtual observatory
    if (_virtualObservatory.Search(_selectedScenario, request, starList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Build the list of calibrator
    sclsvrCALIBRATOR_LIST calibratorList;
   
    // Get the returned star list and create a calibrator list from it
    if (calibratorList.Copy(starList) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    if (_progress.Write("Completing results...", mcsFALSE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Complete the calibrators list
    if (calibratorList.Complete(request) == mcsFAILURE)
    {
        return mcsFAILURE;
    }
    
    if (request.IsNoScienceStar() == mcsTRUE)
    {
        ////////////////////////////////////////////////////////////////////////
        // Remove the science object if it belongs to the calibrator list.
        ////////////////////////////////////////////////////////////////////////
        // 1) Make a copy of the calibrator list in order to create a temp list
        // containing all calibrators within 0.01 ra and dec of the user
        // coordinates
        vobsSTAR_LIST scienceObjects;
        scienceObjects.Copy(calibratorList);
        // 2) Create a filter to only get stars near the original science object
        vobsDISTANCE_FILTER distanceFilter("");
        distanceFilter.SetDistanceValue(request.GetObjectRa(),
                                        request.GetObjectDec(),
                                        0.01, 0.01);
        // 3) Apply the filter to the copied calibrator list
        distanceFilter.Apply(&scienceObjects);

        // 4) Remove from the original calibrator list any star left by the 
        // filter
        // in the temporary list
        vobsSTAR* currentStar = scienceObjects.GetNextStar(mcsTRUE);
        while (currentStar != NULL)
        {
            mcsSTRING32 starId;
            // Get Star ID
            if (currentStar->GetId(starId, sizeof(starId)) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
            logInfo("science star %s has been removed", starId);
            calibratorList.Remove(*currentStar);
            currentStar = scienceObjects.GetNextStar();

        }
        ////////////////////////////////////////////////////////////////////////
    }
    
    if (_progress.Write("Done", mcsTRUE) == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    { 
        string xmlOutput;
        request.AppendParamsToVOTable(xmlOutput);
        char* voHeader = "SearchCal software: http://www.mariotti.fr/aspro_page.htm (In case of problem, please report to jmmc-user-support@ujf-grenoble.fr)";
        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof(softwareVersion), "%s v%s", "SearchCal",
                 sclsvrVERSION);

        // Give back CDATA for msgMESSAGE reply.
        if (msg != NULL)
        {
            calibratorList.Pack(&dynBuff);
        }
        else
        {
            // Otherwise give back a VOTable
            dynBuff.Reset();
            if (calibratorList.GetVOTable(voHeader, softwareVersion, 
                                          requestString, xmlOutput.c_str(),
                                          &dynBuff) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }

        // If a filename has been given, store results as file
        if (strcmp(request.GetFileName(), "") != 0)
        {
            mcsSTRING32 fileName;
            strcpy(fileName, request.GetFileName());
            // If the extension is .vot, save as VO table
            if (strcmp(miscGetExtension(fileName), "vot") == 0)
            {
                // Save the list as a VOTable v1.1
                if (calibratorList.SaveToVOTable
                    (request.GetFileName(), voHeader, softwareVersion,
                     requestString, xmlOutput.c_str()) == mcsFAILURE)
                {
                    return mcsFAILURE;
                }
            }
            else
            {
                if (calibratorList.Save(request.GetFileName(), 
                                    request) == mcsFAILURE)
                {
                    return mcsFAILURE;
                };
            }
        }
    }
    else
    {
        dynBuff.Reset();
    }

    // Wait for the thread only if it had been started
    if (_progress.IsInit() == mcsTRUE)
    {
        // Status monitoring should be done by thread with msgMESSAGE reply.
        if (msg != NULL)
        {
            // Wait for the actionForwarder thread end
            if (thrdThreadWait(&actionMonitor) == mcsFAILURE)
            {
                return mcsFAILURE;
            }
        }
    }

    // sdbAction deletion
    if (_progress.Destroy() == mcsFAILURE)
    {
        return mcsFAILURE;
    }

    // Stop timer log
    timlogStop("GETCAL");
    
    return mcsSUCCESS;
}

evhCB_COMPL_STAT sclsvrSERVER::GetCalCB(msgMESSAGE &msg, void*)
{
    logTrace("sclsvrSERVER::GetCalCB()");

    // Get calibrators
    miscoDYN_BUF dynBuff;
    if (GetCal(msg.GetBody(), dynBuff, &msg) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Set reply
    mcsUINT32 nbStoredBytes;
    dynBuff.GetNbStoredBytes(&nbStoredBytes);
    if (nbStoredBytes != 0)
    {
        msg.SetBody(dynBuff.GetBuffer());
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


/*___oOo___*/
