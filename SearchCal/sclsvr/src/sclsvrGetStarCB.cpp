/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrGetStarCB.cpp,v 1.15 2005-02-07 14:38:45 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    30-Nov-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * sclsvrGetStarCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetStarCB.cpp,v 1.15 2005-02-07 14:38:45 gzins Exp $"; 
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
#include "sclsvrErrors.h"
#include "sclsvrGETSTAR_CMD.h"
#include "sclsvrCALIBRATOR_LIST.h"

/*
 * Class constructor
 */

extern "C"{
#include "simcli.h"
}

/*
 * Class destructor
 */



/*
 * Public methods
 */

evhCB_COMPL_STAT sclsvrSERVER::GetStarCB(msgMESSAGE &msg, void*)
{

    logExtDbg("sclsvrSERVER::GetStarCB()");

    // Search command
    sclsvrGETSTAR_CMD getStarCmd(msg.GetCommand(), msg.GetBody());
    
    // Parse command
    if (getStarCmd.Parse() == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Get star name 
    char *starName;
    if (getStarCmd.GetObjectName(&starName) == mcsFAILURE)
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
    if (simcliGetCoordinates(starName, ra, dec) == mcsFAILURE)
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, starName, "SIMBAD");
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Prepare request to search information in other catalog
    vobsREQUEST request;
    if (request.SetConstraint(STAR_NAME_ID, starName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetConstraint(OBSERVED_BAND_ID, "K") == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    mcsSTRING32 buffer;
    sprintf(buffer, "%f", wlen);
    if (request.SetConstraint(STAR_WLEN_ID, buffer) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    sprintf(buffer, "%f", baseline);
    if (request.SetConstraint(BASEMAX_ID, buffer) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    // Set star
    vobsSTAR star;
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra);
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec);
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    if (_virtualObservatory.Search(request, starList) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    
    // If the star has been found in catalog
    if (starList.Size() != 0)
    {
        // Get first star of the list 
        sclsvrCALIBRATOR calibrator(*starList.GetNextStar(mcsTRUE));

        // Complete missing properties of the calibrator 
        if (calibrator.Complete(request) == mcsFAILURE)
        {
            // Ignore error
            errCloseStack(); 
        }

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

        // Send reply
        if (SendReply(msg) == mcsFAILURE)
        {
            return evhCB_NO_DELETE | evhCB_FAILURE;
        }
    }
    else
    {
        errAdd(sclsvrERR_STAR_NOT_FOUND, starName, "CDS catalogs");
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    return evhCB_NO_DELETE;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */



/*___oOo___*/
