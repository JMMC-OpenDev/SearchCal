/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrGetStarCB.C,v 1.1 2004-11-30 15:48:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    30-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrGetStarCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetStarCB.C,v 1.1 2004-11-30 15:48:09 scetre Exp $"; 
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
    sclsvrGETSTAR_CMD searchCmd(msg.GetCommand(), msg.GetBodyPtr());
    searchCmd.getHelp().data(); 
    
    char *starName;
    if (searchCmd.getObjectName(&starName) == FAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }    
   
    mcsSTRING32 ra, dec;

    // hostname[256], service[8],
    char rec[256] ;
    char *p;
    
    strcat(starName,"!") ; 
    strcpy(rec, starName);
    p=strchr(rec,'_');
    while ( p !=NULL) 
    {
        *p = ' ';
        p=strchr(rec,'_');  
    }

    simcliGetCoordinates(rec, ra, dec);

    vobsREQUEST request;


    if (request.SetConstraint(STAR_NAME_ID,starName) == FAILURE)
    {
        return FAILURE;
    }
    if (request.SetConstraint(OBSERVED_BAND_ID,"V") == FAILURE)
    {
        return FAILURE;
    }
    vobsSTAR star;
    star.SetProperty(POS_EQ_RA_MAIN_ID, ra);
    star.SetProperty(POS_EQ_DEC_MAIN_ID, dec);
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    vobsVIRTUAL_OBSERVATORY vobs;

    if (vobs.Search(request, starList)==FAILURE)
    {
        return FAILURE;
    }
    
    // Build the list of calibrator
    sclsvrCALIBRATOR_LIST calibratorList;

    // get the resulting star list and create a calibrator list
    if (calibratorList.Copy(starList) == FAILURE)
    {
        return FAILURE;
    }
    // complete the calibrators list
    if (calibratorList.Complete(request) == FAILURE)
    {
        return FAILURE;
    }
    calibratorList.Display();

    // Pack the list result in a buffer in order to send it
    miscDYN_BUF dynBuff;
    miscDynBufInit(&dynBuff);
    calibratorList.Pack(&dynBuff);


    msg.SetBody(miscDynBufGetBufferPointer(&dynBuff),
                strlen(miscDynBufGetBufferPointer(&dynBuff)));
    
    calibratorList.Clear();
    starList.Clear();
    miscDynBufDestroy(&dynBuff);
    // Send reply
    if (SendReply(msg) == FAILURE)
    {
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
