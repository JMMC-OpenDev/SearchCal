/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrGetStarCB.cpp,v 1.11 2005-02-02 14:45:56 scetre Exp $"
*
* History
* -------
* scetre    30-Nov-2004  Created
*
*
*******************************************************************************/

/**
 * \file
 * sclsvrGetStarCB class definition.
 */

static char *rcsId="@(#) $Id: sclsvrGetStarCB.cpp,v 1.11 2005-02-02 14:45:56 scetre Exp $"; 
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

    char *starName;
    if (getStarCmd.GetObjectName(&starName) == mcsFAILURE)
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


    if (request.SetConstraint(STAR_NAME_ID,starName) == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    if (request.SetConstraint(OBSERVED_BAND_ID,"V") == mcsFAILURE)
    {
        return evhCB_NO_DELETE | evhCB_FAILURE;
    }
    vobsSTAR star;
    star.SetPropertyValue(vobsSTAR_POS_EQ_RA_MAIN, ra);
    star.SetPropertyValue(vobsSTAR_POS_EQ_DEC_MAIN, dec);
    vobsSTAR_LIST starList;
    starList.AddAtTail(star);
    vobsVIRTUAL_OBSERVATORY vobs;

    if (vobs.Search(request, starList) == mcsFAILURE)
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
    //calibratorList.Display();

    // Pack the list result in a buffer in order to send it
    miscDYN_BUF dynBuff;
    miscDynBufInit(&dynBuff);

    // Table where are stored magnitudes
    mcsSTRING64 starPropertyValue[25];
    // The star asked
    vobsSTAR *tmpStar=(calibratorList.GetNextStar(mcsTRUE));
    // A property table wanted
    int nbProperties = 25;
    mcsSTRING64 starProperty[25] = 
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
        vobsSTAR_ID_ALTERNATIVE,
        vobsSTAR_SPECT_TYPE_MK,
        vobsSTAR_POS_GAL_LAT,
        vobsSTAR_POS_GAL_LON,
        vobsSTAR_POS_PARLX_TRIG,
        sclsvrCALIBRATOR_BO,
        sclsvrCALIBRATOR_VO,
        sclsvrCALIBRATOR_RO,
        sclsvrCALIBRATOR_IO,
        sclsvrCALIBRATOR_JO,
        sclsvrCALIBRATOR_HO,
        sclsvrCALIBRATOR_KO,
        sclsvrCALIBRATOR_LO,
        sclsvrCALIBRATOR_MO,
        sclsvrCALIBRATOR_ANGULAR_DIAMETER,
        sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR,
    };
    // for each property
    for (int i=0; i<nbProperties; i++)
    { 
        strcpy(starPropertyValue[i], tmpStar->GetPropertyValue(starProperty[i]));
    }
   
      
    miscDYN_BUF raBuf;
    miscDynBufInit(&raBuf);
    miscDYN_BUF decBuf;
    miscDynBufInit(&decBuf);
    miscDynBufAppendString(&raBuf, ra);
    miscDynBufAppendString(&decBuf, dec);
    // Replace ' ' by ':' in ra and dec 
    miscReplaceChrByChr(miscDynBufGetBuffer(&raBuf), ' ', ':');
    miscReplaceChrByChr(miscDynBufGetBuffer(&decBuf), ' ', ':');

    // Create the dynamic buffer in which is written the star informations
    miscDynBufAppendString(&dynBuff, starName);
    miscDynBufAppendString(&dynBuff, " 2000.0 ");
    miscDynBufAppendString(&dynBuff, miscDynBufGetBuffer(&raBuf));
    miscDynBufAppendString(&dynBuff, " ");
    miscDynBufAppendString(&dynBuff, miscDynBufGetBuffer(&decBuf));

    for (int i=0; i<nbProperties; i++)
    {
        miscDynBufAppendString(&dynBuff, " ");
        miscDynBufAppendString(&dynBuff, starPropertyValue[i]);
    }
    

    
    msg.SetBody(miscDynBufGetBuffer(&dynBuff),
                strlen(miscDynBufGetBuffer(&dynBuff)));

    miscDynBufDestroy(&dynBuff);
    miscDynBufDestroy(&raBuf);
    miscDynBufDestroy(&decBuf);
    
    //calibratorList.Clear();
    //starList.Clear();
    // Send reply
    if (SendReply(msg) == mcsFAILURE)
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
