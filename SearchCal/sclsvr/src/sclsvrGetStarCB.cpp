/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrGetStarCB.cpp,v 1.5 2005-01-03 14:36:38 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrGetStarCB.cpp,v 1.5 2005-01-03 14:36:38 scetre Exp $"; 
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
    if (getStarCmd.Parse() == FAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }

    char *starName;
    if (getStarCmd.GetObjectName(&starName) == FAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }    
   
    mcsSTRING32 ra, dec;

    // hostname[256], service[8],
    char rec[256] ;
    char *p;
    
    strcpy(rec, starName);
    strcat(rec,"!") ; 
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
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra);
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec);
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
    /*if (calibratorList.Complete(request) == FAILURE)
    {
        return FAILURE;
    }*/
    //calibratorList.Display();

    // Pack the list result in a buffer in order to send it
    miscDYN_BUF dynBuff;
    miscDynBufInit(&dynBuff);

    // Table where are stored magnitudes
    mcsSTRING64 starMagnitudes[10];
    // The star asked
    vobsSTAR *tmpStar=(starList.GetNextStar(mcsTRUE));
    // A property table wanted
    int nbProperties = 10;
    mcsSTRING64 starProperty[10] = 
    {
        vobsSTAR_PHOT_JHN_B,
        vobsSTAR_PHOT_JHN_V,
        vobsSTAR_PHOT_JHN_R,
        vobsSTAR_PHOT_JHN_I,
        vobsSTAR_PHOT_JHN_J,
        vobsSTAR_PHOT_JHN_H,
        vobsSTAR_PHOT_JHN_K,
        vobsSTAR_PHOT_JHN_L,
        vobsSTAR_PHOT_JHN_M,
        vobsSTAR_PHOT_IR_N_10_4
    };
    // for each property
    for (int i=0; i<nbProperties; i++)
    { 
        strcpy(starMagnitudes[i], tmpStar->GetPropertyValue(starProperty[i]));
    }
   
      
    miscDYN_BUF raBuf;
    miscDynBufInit(&raBuf);
    miscDYN_BUF decBuf;
    miscDynBufInit(&decBuf);
    miscDynBufAppendString(&raBuf, ra);
    miscDynBufAppendString(&decBuf, dec);
    // Replace ' ' by ':' in ra and dec 
    miscReplaceChrByChr(miscDynBufGetBufferPointer(&raBuf), ' ', ':');
    miscReplaceChrByChr(miscDynBufGetBufferPointer(&decBuf), ' ', ':');

    // Create the dynamic buffer in which is written the star informations
    miscDynBufAppendString(&dynBuff, starName);
    miscDynBufAppendString(&dynBuff, " 2000.0 ");
    miscDynBufAppendString(&dynBuff, miscDynBufGetBufferPointer(&raBuf));
    miscDynBufAppendString(&dynBuff, " ");
    miscDynBufAppendString(&dynBuff, miscDynBufGetBufferPointer(&decBuf));

    for (int i=0; i<nbProperties; i++)
    {
        miscDynBufAppendString(&dynBuff, " ");
        miscDynBufAppendString(&dynBuff, starMagnitudes[i]);
    }
    

    
    msg.SetBody(miscDynBufGetBufferPointer(&dynBuff),
                strlen(miscDynBufGetBufferPointer(&dynBuff)));

    miscDynBufDestroy(&dynBuff);
    miscDynBufDestroy(&raBuf);
    miscDynBufDestroy(&decBuf);
    
    //calibratorList.Clear();
    //starList.Clear();
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
