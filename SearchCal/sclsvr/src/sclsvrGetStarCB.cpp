/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrGetStarCB.cpp,v 1.2 2004-12-13 13:33:48 scetre Exp $"
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

static char *rcsId="@(#) $Id: sclsvrGetStarCB.cpp,v 1.2 2004-12-13 13:33:48 scetre Exp $"; 
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
    sclsvrGETSTAR_CMD getStarCmd(msg.GetCommand(), msg.GetBodyPtr());
    
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
    //calibratorList.Display();

    // Pack the list result in a buffer in order to send it
    miscDYN_BUF dynBuff;
    miscDynBufInit(&dynBuff);

    // Table where are stored magnitudes
    mcsSTRING32 starMagnitudes[10];
    // The star asked
    vobsSTAR *tmpStar=(starList.GetNextStar(mcsTRUE));
    // A property table wanted
    int nbProperties = 10;
    int starProperty[10] = 
    {
        PHOT_JHN_B_ID,
        PHOT_JHN_V_ID,
        PHOT_JHN_R_ID,
        PHOT_JHN_I_ID,
        PHOT_JHN_J_ID,
        PHOT_JHN_H_ID,
        PHOT_JHN_K_ID,
        PHOT_JHN_L_ID,
        PHOT_JHN_M_ID,
        PHOT_IR_N_10_4_ID
    };
    // for each property
    for (int i=0; i<nbProperties; i++)
    { 
        tmpStar->GetProperty((vobsUCD_ID)starProperty[i], starMagnitudes[i]);
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
    

    printf("%s\n", miscDynBufGetBufferPointer(&dynBuff)); 
    
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
