/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestDistance.c,v 1.1 2005-10-26 11:30:41 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 ******************************************************************************/

/**
 * @file
 * Test program of the function which computes distance.
 */

static char *rcsId="@(#) $Id: alxTestDistance.c,v 1.1 2005-10-26 11:30:41 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


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

    logInfo("Starting...");
   
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == mcsFAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }
    mcsFLOAT ra1;
    mcsFLOAT dec1;
    mcsFLOAT ra2;
    mcsFLOAT dec2;
    mcsFLOAT distance;

    ra1      = 0.0;
    dec1     = 0.0;
    ra2      = 1.0;
    dec2     = 1.0;
    distance = 0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.1f, dec1 = %4.1f, ra2 = %4.1f, dec2 = %4.1f) = %f\n", ra1, dec1, ra2, dec2, distance);
    
    ra1      = -1.0;
    dec1     = -1.0;
    ra2      = 0.0;
    dec2     = 0.0;
    distance = 0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.1f, dec1 = %4.1f, ra2 = %4.1f, dec2 = %4.1f) = %f\n", ra1, dec1, ra2, dec2, distance);
    
    ra1      = -3.0;
    dec1     = -2.0;
    ra2      = -2.0;
    dec2     = -1.0;
    distance = 0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.1f, dec1 = %4.1f, ra2 = %4.1f, dec2 = %4.1f) = %f\n", ra1, dec1, ra2, dec2, distance);
    
    ra1      = 0.0;
    dec1     = 0.0;
    ra2      = 3.0;
    dec2     = 4.0;
    distance = 0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.1f, dec1 = %4.1f, ra2 = %4.1f, dec2 = %4.1f) = %f\n", ra1, dec1, ra2, dec2, distance);
    
    ra1      = 3.0;
    dec1     = 4.0;
    ra2      = 0.0;
    dec2     = 0.0;
    distance = 0.0;
    alxComputeDistance(ra1, dec1, ra2, dec2, &distance);
    printf("alxComputeDistance(ra1 = %4.1f, dec1 = %4.1f, ra2 = %4.1f, dec2 = %4.1f) = %f\n", ra1, dec1, ra2, dec2, distance);
    
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
