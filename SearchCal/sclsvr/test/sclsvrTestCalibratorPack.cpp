/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: sclsvrTestCalibratorPack.cpp,v 1.6 2005-02-15 15:52:00 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * scetre    25-Nov-2004  Created
 *
 ******************************************************************************/


static char *rcsId="@(#) $Id: sclsvrTestCalibratorPack.cpp,v 1.6 2005-02-15 15:52:00 gzins Exp $"; 
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

    logSetStdoutLogLevel(logQUIET);
    
     
    sclsvrCALIBRATOR calibrator1;
    sclsvrCALIBRATOR calibrator2;
    sclsvrCALIBRATOR_LIST calibratorList;
    sclsvrCALIBRATOR_LIST calibratorList2;
    
    
    miscDYN_BUF buffer;
     
    calibrator1.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "4.32");
    calibrator1.SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER, "1.34");
   
    calibrator1.Pack(& buffer);
    calibrator2.UnPack(miscDynBufGetBufferPointer(&buffer));
    calibrator2.SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER_ERROR, "0.01");
    
    calibratorList.AddAtTail(calibrator1);
    calibratorList.AddAtTail(calibrator2);
    
    sclsvrCALIBRATOR calibrator3;
    calibrator3.SetPropertyValue(vobsSTAR_PHOT_JHN_K, "5.52");
    calibrator3.SetPropertyValue(sclsvrCALIBRATOR_ANGULAR_DIAMETER, "1.8");
    calibratorList.AddAtTail(calibrator3); 
    
    calibratorList.Display();

    miscDYN_BUF bufferList;
    // if the Pack and Unpack method are used without pack and unpack method of
    // request object, it is necessary to add a blank line in the bufferList
    miscDynBufAppendString(&bufferList, "\n");
    calibratorList.Pack(& bufferList);    
    printf("pack succeed\n");
    printf("%s\n", miscDynBufGetBufferPointer(&bufferList));
    calibratorList2.UnPack(&bufferList);
    printf("Pack and Unpack in another list :\n");
    calibratorList2.Display();

    // test on pack for request + calib list
    vobsREQUEST request;
    // Set element of the request
    request.SetConstraint(STAR_NAME_ID,"ETA TAU");
    request.SetConstraint(RA_ID,"02+12+52.3");
    request.SetConstraint(DEC_ID,"+06+04+36.2");
    request.SetConstraint(STAR_WLEN_ID,"2.20");
    request.SetConstraint(STAR_MAGNITUDE_ID,"2.50");
    request.SetConstraint(MIN_MAGNITUDE_RANGE_ID,"1");
    request.SetConstraint(MAX_MAGNITUDE_RANGE_ID,"5");
    request.SetConstraint(SEARCH_BOX_RA_ID,"1800");
    request.SetConstraint(SEARCH_BOX_DEC_ID,"300");
    request.SetConstraint(STAR_EXPECTED_VIS_ID,"2");
    request.SetConstraint(STAR_MAX_ERR_VIS_ID,"0.00032");
    request.SetConstraint(OBSERVED_BAND_ID,"K");
    request.SetConstraint(BASEMIN_ID,"46");
    request.SetConstraint(BASEMAX_ID,"100");
    // pack the request in a buffer
    miscDYN_BUF buffer2;
    request.Pack(&buffer2);
    miscDynBufAppendString(&buffer2, "\n");
    printf("value of the buffer2 : \n%s\n", miscDynBufGetBuffer(&buffer2));

    // create a list 
    sclsvrCALIBRATOR_LIST calibList;
    calibList.AddAtTail(calibrator1);
    calibList.AddAtTail(calibrator2);
    calibList.AddAtTail(calibrator3);
    // pack the list in the buffer
    calibList.Pack(&buffer2);
    printf("value of the buffer2 : \n%s\n", miscDynBufGetBuffer(&buffer2));
    
    // build another request from the buffer
    vobsREQUEST request2;
    request2.UnPack(&buffer2);
    printf("value of the buffer2 after unpack : \n%s\n", miscDynBufGetBuffer(&buffer2));
    // pach the request created in another buffer
    miscDYN_BUF buffer3;
    request2.Pack(&buffer3);
    // check that it is a correct request
    printf("value of the buffer3 : \n%s\n", miscDynBufGetBuffer(&buffer3));
    
    
    // create another list 
    sclsvrCALIBRATOR_LIST calibList2;
    // unpack list from the buffer
    calibList2.UnPack(&buffer2);
    
    calibList2.Display();
    printf("calibList2.Size() = %d\n", calibList2.Size());
    // Close MCS services
    mcsExit();
    
    // Exit from the application with mcsSUCCESS
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
