/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrTestCalibratorMethod.cpp,v 1.3 2005-02-15 15:52:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    25-Nov-2004  Created
 *
 ******************************************************************************/


static char *rcsId="@(#) $Id: sclsvrTestCalibratorMethod.cpp,v 1.3 2005-02-15 15:52:00 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <iostream>

/**
 * \namespace std
 * Export standard iostream objects (cin, cout,...).
 */
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "sclsvr.h"
#include "sclsvrPrivate.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main(int argc, char *argv[])
{
    // Initialize MCS services
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with mcsFAILURE
        exit (EXIT_FAILURE);
    }


    logSetStdoutLogLevel(logEXTDBG);
    sclsvrCALIBRATOR calibrator;
    calibrator.SetProperty(ANGULAR_DIAMETER_ID, "1.4");
    
    calibrator.SetProperty(POS_GAL_LAT_ID, "22.23");
    
    calibrator.SetProperty(PHOT_JHN_K_ID, "4.32");
    calibrator.Display(); 
    mcsSTRING32 diamAffectted;
    calibrator.GetProperty(ANGULAR_DIAMETER_ID, diamAffectted);
    mcsFLOAT diamBis;
    calibrator.GetProperty(ANGULAR_DIAMETER_ID, &diamBis);
    mcsFLOAT glat;
    calibrator.GetProperty(POS_GAL_LAT_ID, &glat); 
    mcsFLOAT mgK;
    calibrator.GetProperty(PHOT_JHN_K_ID, &mgK);
    
    sclsvrCALIBRATOR_LIST calibratorList;
    calibratorList.AddAtTail(calibrator);
    calibratorList.Display();
    
    printf("diameter affected and get = %s\n", diamAffectted);
    printf("diameter bis = %f\n", diamBis);
    printf("mag K affected = %f\n", mgK);
    printf("glat affected = %f\n", glat);
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
