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

    printf("alxComputeNewUDFromLDAndSP(1.185, \"K3III\"):\n");
    if (alxComputeNewUDFromLDAndSP(1.185, 212, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    printf("alxComputeNewUDFromLDAndSP(0.966557, \"B7III\"):\n");
    if (alxComputeNewUDFromLDAndSP(0.966557, 68, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    

    printf("alxComputeNewUDFromLDAndSP(1.185, \"ZERTY\"):\n");
    if (alxComputeNewUDFromLDAndSP(1.185, -1 , -1, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        printf("There is a bug in error handling.\n");
    }

    printf("alxComputeNewUDFromLDAndSP(1, \"B7V\"):\n");
    if (alxComputeNewUDFromLDAndSP(1, 68, 5, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    
  
  
    printf("alxComputeNewUDFromLDAndSP(1, \"O4III\"):\n");
    if (alxComputeNewUDFromLDAndSP(1, 16, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    /* should be equivalent to O5III */
    printf("alxComputeNewUDFromLDAndSP(1, \"O4III\"):\n");
    if (alxComputeNewUDFromLDAndSP(1, 16, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }


    printf("alxComputeNewUDFromLDAndSP(1, \"M5III\"):\n");
    if (alxComputeNewUDFromLDAndSP(1, 260, 3, &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    /* NOT equivalent to M5III */
    printf("alxComputeNewUDFromLDAndSP(1, \"M6III\"):\n");
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
