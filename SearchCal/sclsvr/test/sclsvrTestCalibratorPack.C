/*******************************************************************************
* JMMC project
*
* "@(#) $Id: sclsvrTestCalibratorPack.C,v 1.1 2004-11-26 13:53:56 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    25-Nov-2004  Created
*
*
*******************************************************************************/


static char *rcsId="@(#) $Id: sclsvrTestCalibratorPack.C,v 1.1 2004-11-26 13:53:56 scetre Exp $"; 
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
    if (mcsInit(argv[0]) == FAILURE)
    {
        // Error handling if necessary
        
        // Exit from the application with FAILURE
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logQUIET);

    sclsvrCALIBRATOR calibrator1;
    sclsvrCALIBRATOR calibrator2;
    sclsvrCALIBRATOR_LIST calibratorList;
    sclsvrCALIBRATOR_LIST calibratorList2;
    
    
    miscDYN_BUF buffer;

    calibrator1.SetProperty(PHOT_JHN_K_ID, "4.32");
    calibrator1.SetProperty(ANGULAR_DIAMETER_ID, "1.34");
    
    calibrator1.Pack(& buffer);
    calibrator2.UnPack(miscDynBufGetBufferPointer(&buffer));
    calibrator2.SetProperty(ANGULAR_DIAMETER_ERROR_ID, "0.01");
    
    calibratorList.AddAtTail(calibrator1);
    calibratorList.AddAtTail(calibrator2);
    
    sclsvrCALIBRATOR calibrator3;
    calibrator3.SetProperty(PHOT_JHN_K_ID, "5.52");
    calibrator3.SetProperty(ANGULAR_DIAMETER_ID, "1.8");
    calibratorList.AddAtTail(calibrator3);
    
    calibratorList.Display();

    miscDYN_BUF bufferList;
    calibratorList.Pack(& bufferList);    

    calibratorList2.UnPack(&bufferList);
    printf("Pack and Unpack in another list :\n");
    calibratorList2.Display();
    // Close MCS services
    mcsExit();
    
    // Exit from the application with SUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
