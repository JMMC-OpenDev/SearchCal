/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetCalCB.cpp,v 1.14 2005-02-16 16:56:30 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
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
 * Revision 1.7  2005/02/07 14:44:03  gzins
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
 * \file
 * sclsvrGetCalCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetCalCB.cpp,v 1.14 2005-02-16 16:56:30 gzins Exp $"; 
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
#include "sclsvrPrivate.h"
#include "sclsvrCALIBRATOR_LIST.h"
/*
 * Public methods
 */

evhCB_COMPL_STAT sclsvrSERVER::GetCalCB(msgMESSAGE &msg, void*)
{

    logExtDbg("sclsvrSERVER::GetCalCB()");

    // Build the request object from the parameters of the command
    sclsvrREQUEST request;
    if (request.Parse(msg.GetBody()) == mcsFAILURE)
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
    
    // Pack the list result in a buffer in order to send it
    if (calibratorList.Size() != 0)
    { 
        miscoDYN_BUF dynBuff;

        calibratorList.Pack(&dynBuff);
        
        msg.SetBody(dynBuff.GetBuffer());
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
