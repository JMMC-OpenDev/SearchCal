/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Test program of the function which computes distance.
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
 * Main
 */
int main (int argc, char *argv[])
{
    /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    mcsDOUBLE ra1;
    mcsDOUBLE dec1;
    mcsDOUBLE ra2;
    mcsDOUBLE dec2;
    mcsDOUBLE distance;

    ra1      = -176.95;
    dec1     =   45.229;
    ra2      = -177.0;
    dec2     =   45.224;
    distance =    0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.3f, dec1 = %4.3f, ra2 = %4.3f, dec2 = %4.3f) = %f\n", ra1, dec1, ra2, dec2, distance);
    printf("Result should be around 128.1 arcsec accordding to vobsStarSeparation.\n\n");

    ra1      = -177.0;
    dec1     =   45.224;
    ra2      = -176.95;
    dec2     =   45.229;
    distance =    0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.3f, dec1 = %4.3f, ra2 = %4.3f, dec2 = %4.3f) = %f\n", ra1, dec1, ra2, dec2, distance);
    printf("Result should be around 128.1 arcsec accordding to vobsStarSeparation.\n\n");

    ra1      =    0.0;
    dec1     =    0.0;
    ra2      = -176.95;
    dec2     =   45.229;
    distance =    0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.3f, dec1 = %4.3f, ra2 = %4.3f, dec2 = %4.3f) = %f\n", ra1, dec1, ra2, dec2, distance);
    printf("Result should be around 484885.9 arcsec accordding to vobsStarSeparation.\n\n");

    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
