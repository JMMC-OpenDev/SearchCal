/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: alxTestVisibility.c,v 1.2 2005-02-12 15:18:56 gzins Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.1  2005/01/21 08:14:25  gluck
 * Creation
 *
 * 
 * scetre    21-Oct-2004  Created
 *
 ******************************************************************************/

/**
 * \file
 * Test program of the function which computes visibilities.
 */ 

static char *rcsId="@(#) $Id: alxTestVisibility.c,v 1.2 2005-02-12 15:18:56 gzins Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
     /* Configure logging service */
    logSetStdoutLogLevel(logTEST);
    logSetPrintDate(mcsFALSE);
    logSetPrintFileLine(mcsFALSE);

    logInfo("Starting...");
   
    /* Initializes MCS services */
    if (mcsInit(argv[0]) == FAILURE)
    {
        /* Error handling if necessary */
        
        /* Exit from the application with FAILURE */
        exit (EXIT_FAILURE);
    }

    mcsFLOAT angularDiameter=1.35;
    mcsFLOAT angularDiameterError=0.13;
    mcsFLOAT baseMax=100;
    mcsFLOAT wavelength=2.2;
    mcsFLOAT visibility, visibility2;
    mcsFLOAT visibilityError, visibilityError2;
    
    printf("for : \n");
    printf("\t diam       = %f (mas)\n", angularDiameter);
    printf("\t diam error = %f \n", angularDiameterError);
    printf("\t baseMax    = %f (m)\n", baseMax);
    printf("\t wavelength = %f (microm)\n", wavelength);
    
    if (alxComputeVisibility(angularDiameter,
                                angularDiameterError,
                                baseMax,
                                wavelength,
                                &visibility,
                                &visibility2,
                                &visibilityError,
                                &visibilityError2)==FAILURE)
    {
        return FAILURE;
    }
   
    printf("we compute visibility :\n");
    printf("\t   V   = %f\n", visibility);
    printf("\t   V²  = %f\n", visibility2);
    printf("\tdV²/V² = %f\n", visibilityError2);
    logInfo("Exiting...");
    
    /* Close MCS services */
    mcsExit();
    
    /* Exit from the application with SUCCESS */
    exit (EXIT_SUCCESS);
}


/*___oOo___*/
