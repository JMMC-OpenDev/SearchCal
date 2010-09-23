/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetStarCB.cpp,v 1.35 2010-09-23 19:18:41 mella Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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

static char *rcsId __attribute__ ((unused))="@(#) $Id: sclsvrGetStarCB.cpp,v 1.35 2010-09-23 19:18:41 mella Exp $"; 


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

    // Search command
    sclsvrGETSTAR_CMD getStarCmd(msg.GetCommand(), msg.GetBody());
    // Get the request as a string for the case of Save in VOTable
    mcsSTRING256 requestString;
    strncpy(requestString, msg.GetCommand(), 256);


    // Parse command
    if (getStarCmd.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    // Start timer log
    timlogInfoStart(msg.GetCommand());

    // Monitoring task parameters
    sclsvrMONITOR_TASK_PARAMS monitorTaskParams;
    monitorTaskParams.server  = this;
    monitorTaskParams.message = &msg;
    monitorTaskParams.status  = &_status;

    // Monitoring task
    thrdTHREAD_STRUCT       monitorTask;
    monitorTask.function  = sclsvrMonitorTask;
    monitorTask.parameter = (thrdFCT_ARG*)&monitorTaskParams;
    
    // Launch the thread only if SDB had been succesfully started
    if (thrdThreadCreate(&monitorTask) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
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
        miscDYN_BUF reply;
        miscDynBufInit(&reply);
        int propIdx;
        vobsSTAR_PROPERTY *property;
        // Add property name
        for (propIdx = 0; propIdx < calibrator.NbProperties(); propIdx++)
        {
            property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
            miscDynBufAppendString(&reply, property->GetName());
            miscDynBufAppendString(&reply, "\t");
        }
        miscDynBufAppendString(&reply, "\n");
        // Add property value
        for (propIdx = 0; propIdx < calibrator.NbProperties(); propIdx++)
        {
            property = calibrator.GetNextProperty((mcsLOGICAL)(propIdx==0));
            miscDynBufAppendString(&reply, property->GetValue());
            miscDynBufAppendString(&reply, "\t");
        }
        miscDynBufAppendString(&reply, "\n");

        // Send reply
        msg.SetBody(miscDynBufGetBuffer(&reply));
        miscDynBufDestroy(&reply);



        string xmlOutput;
//        request.AppendParamsToVOTable(xmlOutput);
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
        if (SendReply(msg) == mcsFAILURE)
        {
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
        // Wait for the actionForwarder thread end
        if (thrdThreadWait(&monitorTask) == mcsFAILURE)
        {
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
    }


        // Wait for the actionForwarder thread end
        if (thrdThreadWait(&monitorTask) == mcsFAILURE)
        {
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }

    // Stop timer log
    timlogStop(msg.GetCommand());

    return evhCB_NO_DELETE;
}

/*___oOo___*/
