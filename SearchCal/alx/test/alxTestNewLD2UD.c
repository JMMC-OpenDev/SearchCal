/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes uniform-disc diameters from limb-darkened diamter.
 */



/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "alx.h"
#include "alxPrivate.h"


/*
 * Local Variables
 */

 

/* 
 * Signal catching functions  
 */



/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    logSetStdoutLogLevel(logTRACE);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    alxUNIFORM_DIAMETERS ud;
    alxShowUNIFORM_DIAMETERS(&ud);
    alxFlushUNIFORM_DIAMETERS(&ud);
    alxShowUNIFORM_DIAMETERS(&ud);

    printf("Case no conversion values available:\n");
    if (alxComputeNewUDFromLDAndSP(1.00, 26, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    printf("Case first CL values available, lumclass III (Teff=38860):\n");
    if (alxComputeNewUDFromLDAndSP(1.0, 28, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }
   
    printf("Case first CL values available, lumclass V (Teff=41010)\n");
    if (alxComputeNewUDFromLDAndSP(1.0, 28, 5, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    

    printf("Case first N&L values in III, but in lumclass V should switch to Claret: (Teff=7622.53)\n");
    if (alxComputeNewUDFromLDAndSP(1.0, 107 , 5, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    printf("Case first N&L values in III: (Teff=8073.28)\n");
    if (alxComputeNewUDFromLDAndSP(1.0, 107 , 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    printf("Case G2V: (Teff=5790)\n");
    if (alxComputeNewUDFromLDAndSP(1.0, 168, 5, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    
  
  
    printf("case M6 III Claret only, Teff=3200:\n");
    if (alxComputeNewUDFromLDAndSP(1, 264, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }


    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
