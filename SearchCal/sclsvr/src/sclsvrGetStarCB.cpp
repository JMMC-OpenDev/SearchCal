/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetStarCB.cpp,v 1.36 2010-11-10 15:45:16 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.35  2010/09/23 19:18:41  mella
 * add to getstar ability to export in votable
 *
 * Revision 1.34  2007/10/31 11:29:09  gzins
 * Updated to use new sdbENTRY non-blocking class
 *
 * Revision 1.33  2007/06/27 13:00:59  scetre
 * Do not removed science star if present in the resulting list.
 * Updated get star command
 *
 * Revision 1.32  2007/05/15 08:37:16  gzins
 * Fixed bug related to thread synchronisation
 *
 * Revision 1.31  2007/05/11 15:42:46  gzins
 * Fixed bug; missing progressionMessage tread parameter setting
 *
 * Revision 1.30  2006/12/21 15:16:05  lafrasse
 * Updated progression monitoring code (moved from static-based to instance-based).
 *
 * Revision 1.29  2006/10/26 08:15:57  gzins
 * Renamed thrdTHREAD to thrdTHREAD_STRUCT
 *
 * Revision 1.28  2006/03/03 15:25:23  scetre
 * Changed rcsId to rcsId __attribute__ ((unused))
 *
 * Revision 1.27  2006/02/23 16:08:21  lafrasse
 * Added error stack purge in case of bad 'sdb' initialization
 *
 * Revision 1.26  2006/02/22 17:08:33  lafrasse
 * Made it works even if there is no more semaphores left on the running system (no GUI progression in this case)
 *
 * Revision 1.25  2006/02/21 16:52:39  scetre
 * Moved the 2 same method in one in sclsvrSERVER.cpp
 * move the 2 same struct in sclsvrPrivate.h
 *
 * Revision 1.24  2005/12/22 14:39:38  scetre
 * Added sdb and thrd in GetStar
 *
 * Revision 1.23  2005/11/21 13:51:47  scetre
 * Changed bad scenario name
 * Added scenario for single star research -> updated getStar
 *
 * Revision 1.22  2005/11/15 15:01:19  scetre
 * Updated with new scenario structure
 *
 * Revision 1.21  2005/10/26 11:27:24  lafrasse
 * Code review
 *
 * Revision 1.20  2005/06/01 14:18:54  scetre
 * Added filters and filter list objects.
 * Changed logExtDbg to logTrace
 *
 * Revision 1.19  2005/03/07 13:41:18  gzins
 * Remove min baseline length
 *
 * Revision 1.18  2005/03/04 15:38:10  gzins
 * Added timer log
 *
 * Revision 1.17  2005/02/08 20:57:22  gzins
 * Updated call to SetPropertyValue()
 *
 * Revision 1.16  2005/02/08 04:39:32  gzins
 * Updated for new vobsREQUEST API and used new sclsvrREQUEST class
 *
 * Revision 1.15  2005/02/07 14:38:45  gzins
 * Changed GetLambda to GetWlen
 *
 * scetre    30-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * @file
 * sclsvrGetStarCB class definition.
 */

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrGetStarCB.cpp,v 1.36 2010-11-10 15:45:16 lafrasse Exp $"; 


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
 * Local structure
 */

/*
 * Public methods
 */
evhCB_COMPL_STAT sclsvrSERVER::GetStarCB(msgMESSAGE &msg, void*)
{
    logTrace("sclsvrSERVER::GetStarCB()");

    miscoDYN_BUF dynBuf;
    evhCB_COMPL_STAT complStatus;
    complStatus = ProcessGetStarCmd(msg.GetBody(), dynBuf, &msg);

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
mcsCOMPL_STAT sclsvrSERVER::GetStar(const char* query, miscoDYN_BUF &dynBuf)
{
    logTrace("sclsvrSERVER::GetStar()");

    // Get calibrators
    evhCB_COMPL_STAT complStatus;
    complStatus = ProcessGetStarCmd(query, dynBuf, NULL);

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
                                                 miscoDYN_BUF &dynBuf, 
                                                 msgMESSAGE* msg = NULL)
{
    logTrace("sclsvrSERVER::ProcessGetStarCmd()");

    const char* cmdName = "GETSTAR";

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
    thrdTHREAD_STRUCT monitorTask;
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
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
    }

    // Get star name 
    char* objectName;
    if (getStarCmd.GetObjectName(&objectName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }    

    // Get filename 
    char* fileName;
    if (getStarCmd.GetFile(&fileName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }    

    // Get observed wavelength 
    double wlen;
    if (getStarCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }      

    // Get baseline 
    double baseline;
    if (getStarCmd.GetWlen(&baseline) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }        
    
    // Get star position from SIMBAD
    mcsSTRING32 ra, dec;
    if (simcliGetCoordinates(objectName, ra, dec) == mcsFAILURE)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, objectName, "SIMBAD");
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Prepare request to search information in other catalog
    sclsvrREQUEST request;
    if (request.SetObjectName(objectName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetFileName(fileName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetSearchBand("K") ==  mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetObservingWlen(wlen) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetMaxBaselineLength(baseline) ==  mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Set star
    vobsSTAR star;
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra, "");
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec, "");
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    // init the scenario
    if (_scenarioSingleStar.Init(&request, starList) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;        
    }

    if (_virtualObservatory.Search(&_scenarioSingleStar, request,
                                   starList) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
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
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
        //
        //       // Complete missing properties of the calibrator 
        //       if (calibrator.Complete(request) == mcsFAILURE)
        //       {
        //           // Ignore error
        //           errCloseStack(); 
        //       }
        //
        // Prepare reply
        int propIdx;
        vobsSTAR_PROPERTY *property;
        // Add property name
        for (propIdx = 0; propIdx < calibrator.NbProperties(); propIdx++)
        {
            property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
            dynBuf.AppendString(property->GetName());
            dynBuf.AppendString("\t");
        }
        dynBuf.AppendString("\n");
        // Add property value
        for (propIdx = 0; propIdx < calibrator.NbProperties(); propIdx++)
        {
            property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
            dynBuf.AppendString(property->GetValue());
            dynBuf.AppendString("\t");
        }
        dynBuf.AppendString("\n");

        // Send reply
        if (msg != NULL)
        {
            msg->SetBody(dynBuf.GetBuffer());
        }
        else
        {
            printf("%s", dynBuf.GetBuffer());
        }
        dynBuf.Reset();

        string xmlOutput;
        //request.AppendParamsToVOTable(xmlOutput);
        char* voHeader = "Produced by beta version of getStar (In case of problem, please report to jmmc-user-support@ujf-grenoble.fr)";
        // Get the software name and version
        mcsSTRING32 softwareVersion;
        snprintf(softwareVersion, sizeof(softwareVersion), 
                 "%s v%s", "sclsvr", sclsvrVERSION);

        // If a filename has been given, store results as file
        if (strcmp(request.GetFileName(), "") != 0)
        {
            // Save the list as a VOTable v1.1
            if (starList.SaveToVOTable
                (request.GetFileName(), voHeader, softwareVersion,
                 requestString, xmlOutput.c_str()) == mcsFAILURE)
            {
                return evhCB_NO_DELETE | evhCB_FAILURE;
            }
        }

        // Send reply
        if (msg != NULL)
        {
            if (SendReply(*msg) == mcsFAILURE)
            {
                return evhCB_NO_DELETE | evhCB_FAILURE;
            }
            // Wait for the actionForwarder thread end
            if (thrdThreadWait(&monitorTask) == mcsFAILURE)
            {
                return evhCB_NO_DELETE | evhCB_FAILURE;
            }
        }
    }

    if (msg != NULL)
    {
        // Wait for the actionForwarder thread end
        if (thrdThreadWait(&monitorTask) == mcsFAILURE)
        {
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
    }

    // Stop timer log
    timlogStop(cmdName);

    return evhCB_SUCCESS;
}

/*___oOo___*/
