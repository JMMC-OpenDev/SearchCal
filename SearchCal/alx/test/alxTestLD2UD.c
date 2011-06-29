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

    printf("alxComputeUDFromLDAndSP(1.185, \"K3III\"):\n");
    if (alxComputeUDFromLDAndSP(1.185, "K3III", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    printf("alxComputeUDFromLDAndSP(0.966557, \"B7III\"):\n");
    if (alxComputeUDFromLDAndSP(0.966557, "B7III", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    

    printf("alxComputeUDFromLDAndSP(1.185, \"ZERTY\"):\n");
    if (alxComputeUDFromLDAndSP(1.185, "ZERTY", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        printf("There is a bug in error handling.\n");
    }

    printf("alxComputeUDFromLDAndSP(1, \"B7V\"):\n");
    if (alxComputeUDFromLDAndSP(1, "B7V", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    

    printf("alxComputeUDFromLDAndSP(1, \"B7V\"):\n");
    if (alxComputeUDFromLDAndSP(1, "B7V", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }    
  
    printf("alxComputeUDFromLDAndSP(1, \"O5III\"):\n");
    if (alxComputeUDFromLDAndSP(1, "O5III", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    /* should be equivalent to O5III */
    printf("alxComputeUDFromLDAndSP(1, \"O4III\"):\n");
    if (alxComputeUDFromLDAndSP(1, "O4III", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }


    printf("alxComputeUDFromLDAndSP(1, \"M5III\"):\n");
    if (alxComputeUDFromLDAndSP(1, "M5III", &ud) == mcsFAILURE)
    {
        printf("ERROR\n");
    }
    else
    {
        alxShowUNIFORM_DIAMETERS(&ud);
    }

    /* should be equivalent to M5III */
    printf("alxComputeUDFromLDAndSP(1, \"M6III\"):\n");
    if (alxComputeUDFromLDAndSP(1, "M6III", &ud) == mcsFAILURE)
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
