/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetCalCB.cpp,v 1.9 2005-02-08 04:39:32 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.8  2005/02/07 15:01:11  gzins
 * Reformated file header
 *
 * Revision 1.7  2005/02/07 14:44:03  gzins
 * Renamed lambda to wlen.
 * Renamed minMagRange to minDeltaMag, and maxMagRange to maxDeltaMag.
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
 * \file
 * sclsvrGetCalCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetCalCB.cpp,v 1.9 2005-02-08 04:39:32 gzins Exp $"; 
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

#include "vobs.h"

/*
 * Local Headers 
 */
#include "sclsvrSERVER.h"
#include "sclsvrGETCAL_CMD.h"
#include "sclsvrPrivate.h"
#include "sclsvrCALIBRATOR_LIST.h"
/*
 * Public methods
 */

evhCB_COMPL_STAT sclsvrSERVER::GetCalCB(msgMESSAGE &msg, void*)
{

    logExtDbg("sclsvrSERVER::GetCalCB()");

    // GETCAL command
    sclsvrGETCAL_CMD getCalCmd(msg.GetCommand(), msg.GetBody());

    
    // Parse command
    if (getCalCmd.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Object name
    char *objectName;
    if (getCalCmd.GetObjectName(&objectName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Observed magnitude
    mcsDOUBLE magnitude;
    if (getCalCmd.GetMag(&magnitude) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // max calibrator return
    mcsINT32 maxReturn;
    if (getCalCmd.GetMaxReturn(&maxReturn) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // diff ra
    mcsINT32 diffRa;
    if (getCalCmd.GetDiffRa(&diffRa) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    // diff dec
    mcsINT32 diffDec;
    if (getCalCmd.GetDiffDec(&diffDec) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // band
    char *band;
    if (getCalCmd.GetBand(&band) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // minDeltaMag
    mcsDOUBLE minDeltaMag;
    if (getCalCmd.GetMinDeltaMag(&minDeltaMag) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // maxDeltaMag
    mcsDOUBLE maxDeltaMag;
    if (getCalCmd.GetMaxDeltaMag(&maxDeltaMag) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // ra
    char *ra;
    if (getCalCmd.GetRa(&ra) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // dec
    char *dec;
    if (getCalCmd.GetDec(&dec) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // baseMin
    mcsDOUBLE baseMin;
    if (getCalCmd.GetBaseMin(&baseMin) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // baseMax
    mcsDOUBLE baseMax;
    if (getCalCmd.GetBaseMax(&baseMax) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // wlen
    mcsDOUBLE wlen;
    if (getCalCmd.GetWlen(&wlen) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // visibility
    mcsDOUBLE vis;
    if (getCalCmd.GetVis(&vis) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // visibility error
    mcsDOUBLE visErr;
    if (getCalCmd.GetVisErr(&visErr) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    // Build the request object from the parameters of the command
    sclsvrREQUEST request;

    // Affect the reference object name
    if (request.SetObjectName(objectName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the right ascension position
    if (request.SetObjectRa(ra) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the declinaison position
    if (request.SetObjectDec(dec) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the wavelength
    if (request.SetObservingWlen(wlen) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the magnitude
    if (request.SetObjectMag(magnitude) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the min of the magitude range
    if (request.SetMinDeltaMag(minDeltaMag) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the max of the magnitude range
    if (request.SetMaxDeltaMag(maxDeltaMag) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the search box ra
    if (request.SetDeltaRa(diffRa) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the serach box dec
    if (request.SetDeltaDec(diffDec) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the expected visibility
    if (request.SetExpectedVisibility(vis, visErr) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the observed band
    if (request.SetSearchBand(band) ==  mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // Affect the baseline length
    if (request.SetBaseline(baseMin, baseMax) ==  mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Build the list of star which will come from the virtual observatory
    vobsSTAR_LIST starList;

    // start the research in the virtual observatory
    if (_virtualObservatory.Search(request, starList)==mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Build the list of calibrator
    sclsvrCALIBRATOR_LIST calibratorList;
   
    // get the resulting star list and create a calibrator list
    if (calibratorList.Copy(starList) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    // complete the calibrators list
    if (calibratorList.Complete(request) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    calibratorList.Display();
    
    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    { 
        miscDYN_BUF dynBuff;
        miscDynBufInit(&dynBuff);

        calibratorList.Pack(&dynBuff);

        msg.SetBody(miscDynBufGetBuffer(&dynBuff));
    }
    else
    {
        msg.ClearBody();
    }

    // Send reply
    if (SendReply(msg) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    return evhCB_NO_DELETE;
}






/*___oOo___*/
